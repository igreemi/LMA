// ---------------LMA--------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZoomValue = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomSmoothness = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZoomMin = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZoomMax = 1400.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "9999"))
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "9999"))
	float MinStamina = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "9999"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "9999"))
	float StaminaRecoveryRate = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Stamina", meta = (ClampMin = "0", ClampMax = "9999"))
	float StaminaDecreaseRate = 0.02f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float DecreaseInStamina = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRecovery = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Walking", meta = (ClampMin = "0", ClampMax = "999999"))
	float DefaultWalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Character Movement: Walking", meta = (ClampMin = "0", ClampMax = "999999"))
	float SprintSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintCallable)
	bool GetStateSprint();

private:
	float YRotation = -75.0f;
	float ArmLength = 1400.0f;
	float FOV = 55.0f;

	bool bIsSprint = false;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void DecreaseStamina();
	void RecoveryStamina();
	void Sprint();
	void StopSprint();
	void CameraZoomUp();
	void CameraZoomDown();
	void OnDeath();
	void RotationPlayerOnCursor();
	void OnHealthChanged(float NewHealth);
	FTimerHandle DecreaseStaminaTimer;
	FTimerHandle RecoveryStaminaTimer;
};
