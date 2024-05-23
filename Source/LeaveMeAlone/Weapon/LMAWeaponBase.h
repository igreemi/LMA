// ---------------LMA--------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMAWeaponBase.generated.h"

class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FAmmoWeapon
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Bullets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 Clips;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool Infinite;

	FAmmoWeapon(const int32& Bullets, const int32& Clips, bool Infinite) : Bullets(Bullets), Clips(Clips), Infinite(Infinite) {}

	FAmmoWeapon() = default;
};

UCLASS()
class LEAVEMEALONE_API ALMAWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALMAWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	USkeletalMeshComponent* WeaponComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float TraceDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FAmmoWeapon AmmoWeapon{30, 0, true};

	FAmmoWeapon CurrentAmmoWeapon;

	void Shoot();
	void DecrementBullets();
	bool IsCurrentClipEmpty() const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Fire();
	void ChangeClip();
};
