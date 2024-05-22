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
	if (IsDead()) // ��� 4 - ���� �������� ���������, 
		return;   // ������� �� �������, ����� �� �������� �������� ������� OnDeath

	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth); // ��� 1 - �������� ����
	OnHealthChanged.Broadcast(Health); // ��� 2 - �������� ������� OnHealthChanged

	if (IsDead()) // ��� 3 - ���� �������� ��������� �������� ������� OnDeath
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
