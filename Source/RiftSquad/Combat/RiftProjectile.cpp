#include "Combat/RiftProjectile.h"
#include "Combat/RiftHealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ARiftProjectile::ARiftProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetSphereRadius(12.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &ARiftProjectile::OnProjectileHit);
    RootComponent = CollisionComponent;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1800.0f;
    ProjectileMovement->MaxSpeed = 1800.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;

    Damage = 12.0f;
    LifeSeconds = 3.0f;
}

void ARiftProjectile::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(LifeSeconds);
}

void ARiftProjectile::InitializeProjectile(AActor* InInstigatorActor, float InDamage)
{
    InstigatorActor = InInstigatorActor;
    Damage = InDamage;
}

void ARiftProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!HasAuthority() || !OtherActor || OtherActor == GetOwner() || OtherActor == InstigatorActor)
    {
        return;
    }

    if (URiftHealthComponent* HealthComponent = OtherActor->FindComponentByClass<URiftHealthComponent>())
    {
        HealthComponent->ApplyDamage(Damage, InstigatorActor ? InstigatorActor : GetOwner());
    }

    Destroy();
}
