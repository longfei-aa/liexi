#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiftAbilityPulse.generated.h"

class UStaticMeshComponent;

UCLASS()
class RIFTSQUAD_API ARiftAbilityPulse : public AActor
{
    GENERATED_BODY()

public:
    ARiftAbilityPulse();

    void ConfigurePulse(float Radius, float Lifetime);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Ability")
    TObjectPtr<UStaticMeshComponent> VisualMesh;
};
