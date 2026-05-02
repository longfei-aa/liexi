#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RiftWeaponComponent.generated.h"

class ARiftProjectile;

UCLASS(ClassGroup = (Rift), meta = (BlueprintSpawnableComponent))
class RIFTSQUAD_API URiftWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URiftWeaponComponent();

    UFUNCTION(BlueprintCallable, Category = "Rift|Weapon")
    void RequestFire(FVector Origin, FVector Direction);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Weapon")
    TSubclassOf<ARiftProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Weapon")
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Weapon")
    float FireInterval;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Weapon")
    float MuzzleOffset;

    float LastFireTime;

    UFUNCTION(Server, Reliable)
    void ServerRequestFire(FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction);

    void FireInternal(FVector Origin, FVector Direction);
};
