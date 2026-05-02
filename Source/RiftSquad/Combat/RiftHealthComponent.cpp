#include "Combat/RiftHealthComponent.h"
#include "Net/UnrealNetwork.h"

URiftHealthComponent::URiftHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    bIsDead = false;
}

void URiftHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        if (Owner->HasAuthority())
        {
            CurrentHealth = MaxHealth;
        }
    }
}

void URiftHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URiftHealthComponent, MaxHealth);
    DOREPLIFETIME(URiftHealthComponent, CurrentHealth);
    DOREPLIFETIME(URiftHealthComponent, bIsDead);
}

bool URiftHealthComponent::ApplyDamage(float DamageAmount, AActor* DamageInstigator)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority() || bIsDead || DamageAmount <= 0.0f)
    {
        return false;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
    const float Delta = CurrentHealth - PreviousHealth;

    OnHealthChanged.Broadcast(this, CurrentHealth, Delta);

    if (CurrentHealth <= 0.0f && !bIsDead)
    {
        bIsDead = true;
        OnDeath.Broadcast(this, DamageInstigator);
    }

    return !FMath::IsNearlyZero(Delta);
}

void URiftHealthComponent::ResetHealth()
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
    {
        return;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = MaxHealth;
    bIsDead = false;
    OnHealthChanged.Broadcast(this, CurrentHealth, CurrentHealth - PreviousHealth);
}

void URiftHealthComponent::SetMaxHealth(float NewMaxHealth, bool bFillHealth)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
    {
        return;
    }

    const float PreviousHealth = CurrentHealth;
    MaxHealth = FMath::Max(1.0f, NewMaxHealth);
    CurrentHealth = bFillHealth ? MaxHealth : FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
    bIsDead = CurrentHealth <= 0.0f;
    OnHealthChanged.Broadcast(this, CurrentHealth, CurrentHealth - PreviousHealth);
}

void URiftHealthComponent::OnRep_CurrentHealth(float PreviousHealth)
{
    OnHealthChanged.Broadcast(this, CurrentHealth, CurrentHealth - PreviousHealth);
}

void URiftHealthComponent::OnRep_IsDead()
{
    if (bIsDead)
    {
        OnDeath.Broadcast(this, nullptr);
    }
}
