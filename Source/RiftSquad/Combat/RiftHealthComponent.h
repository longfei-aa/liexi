#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RiftHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRiftHealthChangedSignature, URiftHealthComponent*, HealthComponent, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRiftDeathSignature, URiftHealthComponent*, HealthComponent, AActor*, DamageInstigator);

UCLASS(ClassGroup = (Rift), meta = (BlueprintSpawnableComponent))
class RIFTSQUAD_API URiftHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URiftHealthComponent();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Rift|Health")
    bool ApplyDamage(float DamageAmount, AActor* DamageInstigator);

    UFUNCTION(BlueprintCallable, Category = "Rift|Health")
    void ResetHealth();

    UFUNCTION(BlueprintCallable, Category = "Rift|Health")
    void SetMaxHealth(float NewMaxHealth, bool bFillHealth);

    UFUNCTION(BlueprintPure, Category = "Rift|Health")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Rift|Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Rift|Health")
    bool IsDead() const { return bIsDead; }

    UPROPERTY(BlueprintAssignable, Category = "Rift|Health")
    FRiftHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Rift|Health")
    FRiftDeathSignature OnDeath;

protected:
    UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Rift|Health")
    float MaxHealth;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "Rift|Health")
    float CurrentHealth;

    UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadOnly, Category = "Rift|Health")
    bool bIsDead;

    UFUNCTION()
    void OnRep_CurrentHealth(float PreviousHealth);

    UFUNCTION()
    void OnRep_IsDead();
};
