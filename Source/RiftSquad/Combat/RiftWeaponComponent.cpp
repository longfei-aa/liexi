#include "Combat/RiftWeaponComponent.h"
#include "Combat/RiftProjectile.h"

URiftWeaponComponent::URiftWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

    Damage = 12.0f;
    FireInterval = 0.35f;
    MuzzleOffset = 70.0f;
    LastFireTime = -1000.0f;
    ProjectileClass = ARiftProjectile::StaticClass();
}

void URiftWeaponComponent::RequestFire(FVector Origin, FVector Direction)
{
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    const FVector SafeDirection = Direction.GetSafeNormal();
    if (Owner->HasAuthority())
    {
        FireInternal(Origin, SafeDirection);
    }
    else
    {
        ServerRequestFire(Origin, SafeDirection);
    }
}

void URiftWeaponComponent::ServerRequestFire_Implementation(FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction)
{
    FireInternal(Origin, Direction);
}

void URiftWeaponComponent::FireInternal(FVector Origin, FVector Direction)
{
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !Owner || !Owner->HasAuthority() || !ProjectileClass)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();
    if (Now - LastFireTime < FireInterval)
    {
        return;
    }

    LastFireTime = Now;

    const FVector SafeDirection = Direction.GetSafeNormal();
    const FVector SpawnLocation = Origin + SafeDirection * MuzzleOffset + FVector(0.0f, 0.0f, 40.0f);
    const FRotator SpawnRotation = SafeDirection.Rotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Cast<APawn>(Owner);

    ARiftProjectile* Projectile = World->SpawnActor<ARiftProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
    if (Projectile)
    {
        Projectile->InitializeProjectile(Owner, Damage);
    }
}
