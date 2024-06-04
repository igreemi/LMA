// ---------------LMA--------------

#include "LMADefaultCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "../Component/LMAHealthComponent.h"
#include "../Component/LMAWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LMAPlayerController.h"
#include <LeaveMeAlone/Widgets/BaseHUD.h>

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");

	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);

	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
}

bool ALMADefaultCharacter::GetStateSprint()
{
	if (GetVelocity().Size() > DefaultWalkSpeed && bIsSprint == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(HealthComponent->IsDead()))
	{
		RotationPlayerOnCursor();
	}
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprint);

	PlayerInputComponent->BindAction("CameraZoomUp", IE_Pressed, this, &ALMADefaultCharacter::CameraZoomUp);
	PlayerInputComponent->BindAction("CameraZoomDown", IE_Pressed, this, &ALMADefaultCharacter::CameraZoomDown);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::StopFire);

	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	float Direction = GetActorRotation().Yaw;

	if (Direction == FMath::Clamp(Direction, -45.f, 45.f))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
	else if (Direction == FMath::Clamp(Direction, 45.f, 115.f))
	{
		AddMovementInput(GetActorRightVector(), -Value);
	}
	else if (Direction == FMath::Clamp(Direction, 115.f, 180.f) || Direction == FMath::Clamp(Direction, -180.f, -135.f))
	{
		AddMovementInput(GetActorForwardVector(), -Value);
	}
	else if (Direction == FMath::Clamp(Direction, -135.f, -45.f))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	float Direction = GetActorRotation().Yaw;

	if (Direction == FMath::Clamp(Direction, -45.f, 45.f))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
	else if (Direction == FMath::Clamp(Direction, 115.f, 180.f) || Direction == FMath::Clamp(Direction, -180.f, -135.f))
	{
		AddMovementInput(GetActorRightVector(), -Value);
	}
	else if (Direction == FMath::Clamp(Direction, 45.f, 115.f))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
	else if (Direction == FMath::Clamp(Direction, -135.f, -45.f))
	{
		AddMovementInput(GetActorForwardVector(), -Value);
	}
}

void ALMADefaultCharacter::DecreaseStamina()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("(Stamina > MinStamina) = %f"), Stamina));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("GetVelocity().Size() = %f"), GetVelocity().Size()));

	if (Stamina > MinStamina && GetVelocity().Size() > DefaultWalkSpeed)
	{
		Stamina = FMath::Max(MinStamina, Stamina - DecreaseInStamina);
	}
	else if (FMath::IsNearlyEqual(Stamina, MinStamina))
	{
		Stamina = MinStamina;

		bIsSprint = false;

		if (GetWorldTimerManager().IsTimerActive(DecreaseStaminaTimer))
		{
			GetWorldTimerManager().ClearTimer(DecreaseStaminaTimer);
		}
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void ALMADefaultCharacter::RecoveryStamina()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("(Stamina < MaxStamina) = %f"), Stamina));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("GetVelocity().Size() = %f"), GetVelocity().Size()));

	if (Stamina < MaxStamina && GetVelocity().Size() <= DefaultWalkSpeed)
	{
		Stamina = FMath::Min(MaxStamina, Stamina + StaminaRecovery);
	}
	else if (FMath::IsNearlyEqual(Stamina, MaxStamina))
	{
		Stamina = MaxStamina;

		if (GetWorldTimerManager().IsTimerActive(RecoveryStaminaTimer))
		{
			GetWorldTimerManager().ClearTimer(RecoveryStaminaTimer);
		}
	}
}

void ALMADefaultCharacter::Sprint()
{
	if (GetWorldTimerManager().IsTimerActive(RecoveryStaminaTimer))
	{
		GetWorldTimerManager().ClearTimer(RecoveryStaminaTimer);
	}

	bIsSprint = true;

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	if (!GetWorldTimerManager().IsTimerActive(DecreaseStaminaTimer))
	{
		GetWorldTimerManager().SetTimer(DecreaseStaminaTimer, this, &ThisClass::DecreaseStamina, StaminaDecreaseRate, true);
	}
}

void ALMADefaultCharacter::StopSprint()
{
	if (GetWorldTimerManager().IsTimerActive(DecreaseStaminaTimer))
	{
		GetWorldTimerManager().ClearTimer(DecreaseStaminaTimer);
	}

	bIsSprint = false;

	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;

	if (!GetWorldTimerManager().IsTimerActive(RecoveryStaminaTimer))
	{
		GetWorldTimerManager().SetTimer(RecoveryStaminaTimer, this, &ThisClass::RecoveryStamina, StaminaRecoveryRate, true);
	}
}

void ALMADefaultCharacter::CameraZoomUp()
{
	UE_LOG(LogTemp, Display, TEXT("CameraZoomUp"));
	if (CameraZoomMin < SpringArmComponent->TargetArmLength)
	{
		float TargetZoomTMP = SpringArmComponent->TargetArmLength - CameraZoomValue;
		SpringArmComponent->TargetArmLength =
			FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoomTMP, GetWorld()->TimeSeconds, 1.0f);
	}
}

void ALMADefaultCharacter::CameraZoomDown()
{
	UE_LOG(LogTemp, Display, TEXT("CameraZoomDown"));
	if (CameraZoomMax > SpringArmComponent->TargetArmLength)
	{
		float TargetZoomTMP = SpringArmComponent->TargetArmLength + CameraZoomValue;
		SpringArmComponent->TargetArmLength =
			FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoomTMP, GetWorld()->TimeSeconds, 1.0f);
	}
}

void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();

	auto PlayerController = Cast<ALMAPlayerController>(Controller);
	if (PlayerController)
	{
		auto HUD = Cast<ABaseHUD>(PlayerController->GetHUD());
		if (HUD)
		{
			for (auto Widget : HUD->WidgetsContainer)
			{
				Widget->RemoveFromParent();
			}
		}
	}

	PlayAnimMontage(DeathMontage);
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(5.0f);
	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

/*
void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}
*/