#include "Abilities/RiftAbilityPulse.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ARiftAbilityPulse::ARiftAbilityPulse()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
    SetReplicateMovement(true);

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    RootComponent = VisualMesh;
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetRelativeScale3D(FVector(1.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PulseMeshFinder(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (PulseMeshFinder.Succeeded())
    {
        VisualMesh->SetStaticMesh(PulseMeshFinder.Object);
    }
}

void ARiftAbilityPulse::ConfigurePulse(float Radius, float Lifetime)
{
    const float VisualScale = FMath::Max(0.1f, Radius / 100.0f);
    SetActorScale3D(FVector(VisualScale, VisualScale, 0.08f));
    SetLifeSpan(FMath::Max(0.05f, Lifetime));
}
