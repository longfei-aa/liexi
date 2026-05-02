#include "Combat/RiftProjectile.h"
#include "Combat/RiftHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

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

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetRelativeScale3D(FVector(0.22f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (ProjectileMeshFinder.Succeeded())
    {
        VisualMesh->SetStaticMesh(ProjectileMeshFinder.Object);
    }

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
        AActor* DamageSource = InstigatorActor.Get();
        HealthComponent->ApplyDamage(Damage, DamageSource ? DamageSource : GetOwner());
    }

    Destroy();
}
