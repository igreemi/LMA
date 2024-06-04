// ---------------LMA--------------

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <LeaveMeAlone/Weapon/LMAWeaponBase.h>
#include "LMAWeaponComponent.generated.h"


class ALMAWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULMAWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ALMAWeaponBase> WeaponClass;

	UPROPERTY()
	ALMAWeaponBase* Weapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireRate = 0.5f;

	bool AnimReloading = false;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Fire();
	void StartFire();
	void StopFire();
	void Reload();
	void SpawnWeapon();
	void InitAnimNotify();
	void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);
	bool CanReload() const;
	void OutOfBullets();

	UFUNCTION(BlueprintCallable)
	bool GetCurrentWeapnAmmo(FAmmoWeapon& AmmoWeapon) const; 

private:
	FTimerHandle FireTimer;
	bool bFireButtonOn = false;

};
