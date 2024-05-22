// ---------------LMA--------------

#include "LMAHealthComponent.h"

// Sets default values for this component's properties
ULMAHealthComponent::ULMAHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool ULMAHealthComponent::IsDead() const
{
	return Health <= 0.0f;
}

// Called when the game starts
void ULMAHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	OnHealthChanged.Broadcast(Health);

	AActor* OwnerComponent = GetOwner();
	if (OwnerComponent)
	{
		OwnerComponent->OnTakeAnyDamage.AddDynamic(this, &ULMAHealthComponent::OnTakeAnyDamage);
	}
}

void ULMAHealthComponent::OnTakeAnyDamage(
	AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (IsDead()) // шаг 4 - если здоровье кончилось, 
		return;   // выходим из функции, чтобы не вызывать повторно делегат OnDeath

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth); // шаг 1 - получаем урон
	OnHealthChanged.Broadcast(Health); // шаг 2 - вызываем делегат OnHealthChanged

	if (IsDead()) // шаг 3 - если здоровье кончилось вызываем делегат OnDeath
	{
		OnDeath.Broadcast();
	}
}

bool ULMAHealthComponent::AddHealth(float NewHealth)
{
	if (IsDead() || IsHealthFull())
		return false;
	Health = FMath::Clamp(Health + NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
	return true;
}

bool ULMAHealthComponent::IsHealthFull() const
{
	return FMath::IsNearlyEqual(Health, MaxHealth);
}
