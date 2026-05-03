#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RiftAbilityComponent.generated.h"

class ARiftAbilityPulse;

UCLASS(ClassGroup = (Rift), meta = (BlueprintSpawnableComponent))
class RIFTSQUAD_API URiftAbilityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URiftAbilityComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Rift|Ability")
    void RequestShockwave(FVector Origin);

    UFUNCTION(BlueprintCallable, Category = "Rift|Ability")
    void RequestDash(FVector Direction);

    UFUNCTION(BlueprintPure, Category = "Rift|Ability")
    float GetShockwaveCooldownRemaining() const;

    UFUNCTION(BlueprintPure, Category = "Rift|Ability")
    float GetDashCooldownRemaining() const;

protected:
    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Rift|Ability")
    float ShockwaveDamage;

    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Rift|Ability")
    float ShockwaveRadius;

    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Rift|Ability")
    float ShockwaveCooldown;

    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Rift|Ability")
    float DashDistance;

    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Rift|Ability")
    float DashCooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Ability")
    TSubclassOf<ARiftAbilityPulse> PulseClass;

    UPROPERTY(Replicated)
    float LastShockwaveTime;

    UPROPERTY(Replicated)
    float LastDashTime;

    UFUNCTION(Server, Reliable)
    void ServerRequestShockwave(FVector_NetQuantize Origin);

    UFUNCTION(Server, Reliable)
    void ServerRequestDash(FVector_NetQuantizeNormal Direction);

    bool CanUseShockwave() const;
    bool CanUseDash() const;
    bool IsOwnerAbleToAct() const;
    void ExecuteShockwave(FVector Origin);
    void ExecuteDash(FVector Direction);
};
