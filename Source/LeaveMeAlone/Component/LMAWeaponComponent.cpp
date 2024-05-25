// ---------------LMA--------------
#include "LMAWeaponComponent.h"
#include "../Animations/LMAReloadFinishedAnimNotify.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "../Weapon/LMAWeaponBase.h"

// Sets default values for this component's properties
ULMAWeaponComponent::ULMAWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();

	InitAnimNotify();

	Weapon->OutOfBullets.AddUObject(this, &ULMAWeaponComponent::OutOfBullets);
}

void ULMAWeaponComponent::SpawnWeapon()
{
	Weapon = GetWorld()->SpawnActor<ALMAWeaponBase>(WeaponClass);
	if (Weapon)
	{
		const auto Character = Cast<ACharacter>(GetOwner());
		if (Character)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
			Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
		}
	}
}

void ULMAWeaponComponent::Reload()
{

		if (FireTimer.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimer);
		}

		OutOfBullets();

		if (!FireTimer.IsValid() && bFireButtonOn)
		{
			GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ThisClass::Fire, FireRate, true, 0);
		}
	
}

void ULMAWeaponComponent::InitAnimNotify()
{
	if (!ReloadMontage)
		return;

	const auto NotifiesEvents = ReloadMontage->Notifies;
	for (auto NotifyEvent : NotifiesEvents)
	{
		auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
		if (ReloadFinish)
		{
			ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
			break;
		}
	}
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
	const auto Character = Cast<ACharacter>(GetOwner());
	if (Character->GetMesh() == SkeletalMesh)
	{
		AnimReloading = false;
	}
}

bool ULMAWeaponComponent::CanReload() const
{
	if (!Weapon->FullOrEmptyClip())
	{
		return 1;
	}
	return !AnimReloading;
}

void ULMAWeaponComponent::OutOfBullets()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("OutOfBullets")));

	if (!CanReload())
	{
		return;
	}

	Weapon->ChangeClip();
	AnimReloading = true;
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	Character->PlayAnimMontage(ReloadMontage);
}

void ULMAWeaponComponent::Fire()
{
	if (Weapon && !AnimReloading)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("AnimReloading = %i"), AnimReloading));

		Weapon->Fire();
	}
}

void ULMAWeaponComponent::StartFire()
{
	bFireButtonOn = true;

	if (FireTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	}

	if (!FireTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ThisClass::Fire, FireRate, true, 0);
	}
}

void ULMAWeaponComponent::StopFire()
{
	bFireButtonOn = false;

	if (FireTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
	}
}

// Called every frame
void ULMAWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
