#include "Abilities/RiftAbilityComponent.h"

#include "Abilities/RiftAbilityPulse.h"
#include "Combat/RiftHealthComponent.h"
#include "Enemies/RiftEnemyBase.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

URiftAbilityComponent::URiftAbilityComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    ShockwaveDamage = 38.0f;
    ShockwaveRadius = 320.0f;
    ShockwaveCooldown = 5.0f;
    DashDistance = 360.0f;
    DashCooldown = 3.0f;
    LastShockwaveTime = -1000.0f;
    LastDashTime = -1000.0f;
    PulseClass = ARiftAbilityPulse::StaticClass();
}

void URiftAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URiftAbilityComponent, ShockwaveDamage);
    DOREPLIFETIME(URiftAbilityComponent, ShockwaveRadius);
    DOREPLIFETIME(URiftAbilityComponent, ShockwaveCooldown);
    DOREPLIFETIME(URiftAbilityComponent, DashDistance);
    DOREPLIFETIME(URiftAbilityComponent, DashCooldown);
    DOREPLIFETIME(URiftAbilityComponent, LastShockwaveTime);
    DOREPLIFETIME(URiftAbilityComponent, LastDashTime);
}

void URiftAbilityComponent::RequestShockwave(FVector Origin)
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    if (Owner->HasAuthority())
    {
        ExecuteShockwave(Origin);
    }
    else
    {
        ServerRequestShockwave(Origin);
    }
}

void URiftAbilityComponent::RequestDash(FVector Direction)
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    const FVector SafeDirection = Direction.GetSafeNormal2D();
    if (Owner->HasAuthority())
    {
        ExecuteDash(SafeDirection);
    }
    else
    {
        ServerRequestDash(SafeDirection);
    }
}

float URiftAbilityComponent::GetShockwaveCooldownRemaining() const
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return 0.0f;
    }

    return FMath::Max(0.0f, ShockwaveCooldown - (World->GetTimeSeconds() - LastShockwaveTime));
}

float URiftAbilityComponent::GetDashCooldownRemaining() const
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return 0.0f;
    }

    return FMath::Max(0.0f, DashCooldown - (World->GetTimeSeconds() - LastDashTime));
}

void URiftAbilityComponent::MultiplyCooldowns(float Multiplier)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
    {
        return;
    }

    ShockwaveCooldown = FMath::Clamp(ShockwaveCooldown * Multiplier, 1.0f, 20.0f);
    DashCooldown = FMath::Clamp(DashCooldown * Multiplier, 0.5f, 12.0f);
}

void URiftAbilityComponent::AddShockwaveDamage(float BonusDamage)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
    {
        return;
    }

    ShockwaveDamage = FMath::Max(1.0f, ShockwaveDamage + BonusDamage);
}

void URiftAbilityComponent::MultiplyShockwaveRadius(float Multiplier)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
    {
        return;
    }

    ShockwaveRadius = FMath::Clamp(ShockwaveRadius * Multiplier, 120.0f, 900.0f);
}

void URiftAbilityComponent::ServerRequestShockwave_Implementation(FVector_NetQuantize Origin)
{
    ExecuteShockwave(Origin);
}

void URiftAbilityComponent::ServerRequestDash_Implementation(FVector_NetQuantizeNormal Direction)
{
    ExecuteDash(Direction);
}

bool URiftAbilityComponent::CanUseShockwave() const
{
    return IsOwnerAbleToAct() && GetShockwaveCooldownRemaining() <= 0.0f;
}

bool URiftAbilityComponent::CanUseDash() const
{
    return IsOwnerAbleToAct() && GetDashCooldownRemaining() <= 0.0f;
}

bool URiftAbilityComponent::IsOwnerAbleToAct() const
{
    const AActor* Owner = GetOwner();
    const URiftHealthComponent* HealthComponent = Owner ? Owner->FindComponentByClass<URiftHealthComponent>() : nullptr;
    return Owner && Owner->HasAuthority() && (!HealthComponent || !HealthComponent->IsDead());
}

void URiftAbilityComponent::ExecuteShockwave(FVector Origin)
{
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !Owner || !CanUseShockwave())
    {
        return;
    }

    LastShockwaveTime = World->GetTimeSeconds();

    FCollisionShape ShockwaveShape = FCollisionShape::MakeSphere(ShockwaveRadius);
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RiftShockwave), false, Owner);

    TArray<FOverlapResult> Overlaps;
    World->OverlapMultiByObjectType(
        Overlaps,
        Origin,
        FQuat::Identity,
        FCollisionObjectQueryParams(ECC_Pawn),
        ShockwaveShape,
        QueryParams);

    TSet<AActor*> DamagedActors;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        ARiftEnemyBase* Enemy = Cast<ARiftEnemyBase>(Overlap.GetActor());
        if (!Enemy || DamagedActors.Contains(Enemy))
        {
            continue;
        }

        if (URiftHealthComponent* HealthComponent = Enemy->FindComponentByClass<URiftHealthComponent>())
        {
            HealthComponent->ApplyDamage(ShockwaveDamage, Owner);
            DamagedActors.Add(Enemy);
        }
    }

    if (PulseClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Owner;
        SpawnParams.Instigator = Cast<APawn>(Owner);
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ARiftAbilityPulse* Pulse = World->SpawnActor<ARiftAbilityPulse>(PulseClass, Origin + FVector(0.0f, 0.0f, 24.0f), FRotator::ZeroRotator, SpawnParams);
        if (Pulse)
        {
            Pulse->ConfigurePulse(ShockwaveRadius, 0.35f);
        }
    }
}

void URiftAbilityComponent::ExecuteDash(FVector Direction)
{
    UWorld* World = GetWorld();
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!World || !OwnerCharacter || !CanUseDash())
    {
        return;
    }

    const FVector SafeDirection = Direction.IsNearlyZero() ? OwnerCharacter->GetActorForwardVector().GetSafeNormal2D() : Direction.GetSafeNormal2D();
    const FVector StartLocation = OwnerCharacter->GetActorLocation();
    const FVector TargetLocation = StartLocation + SafeDirection * DashDistance;

    FHitResult SweepHit;
    OwnerCharacter->SetActorLocation(TargetLocation, true, &SweepHit, ETeleportType::TeleportPhysics);
    LastDashTime = World->GetTimeSeconds();
}
