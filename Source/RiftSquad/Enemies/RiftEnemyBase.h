#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "GameFramework/Character.h"
#include "RiftEnemyBase.generated.h"

class ARiftRoomManager;
class ARiftProjectile;
class URiftHealthComponent;
class UStaticMeshComponent;

UCLASS()
class RIFTSQUAD_API ARiftEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    ARiftEnemyBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    void SetOwningRoom(ARiftRoomManager* InRoomManager);

    UFUNCTION(BlueprintCallable, Category = "Rift|Enemy")
    void ConfigureEnemy(ERiftEnemyType NewEnemyType, float NewMaxHealth, float NewMoveSpeed, float NewAttackDamage, float NewAttackCooldown, FVector NewVisualScale);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Visual")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float MoveSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float AttackRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float AttackDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float AttackCooldown;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float ActivationDelay;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float PreferredRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float ExplosionRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    float ExplosionFuseSeconds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    TSubclassOf<ARiftProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Enemy")
    ERiftEnemyType EnemyType;

    UPROPERTY()
    TObjectPtr<ARiftRoomManager> OwningRoomManager;

    float LastAttackTime;
    float SpawnTime;
    bool bExplosionTriggered;
    FVector BaseVisualScale;
    FTimerHandle HitFeedbackTimerHandle;

    APawn* FindTargetPawn() const;
    void TickChaser(APawn* TargetPawn, float Distance, FVector ToTarget);
    void TickShooter(APawn* TargetPawn, float Distance, FVector ToTarget);
    void TickBurster(APawn* TargetPawn, float Distance, FVector ToTarget);
    void TryAttack(APawn* TargetPawn);
    void FireProjectileAt(APawn* TargetPawn);
    void TriggerExplosion(APawn* TargetPawn);
    void ExecuteExplosion();
    void ApplyVisualForType();
    void ResetHitFeedback();

    UFUNCTION()
    void HandleHealthChanged(URiftHealthComponent* ChangedHealthComponent, float NewHealth, float Delta);

    UFUNCTION()
    void HandleDeath(URiftHealthComponent* DeadHealthComponent, AActor* DamageInstigator);

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastPlayHitFeedback();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayDeathFeedback();
};
