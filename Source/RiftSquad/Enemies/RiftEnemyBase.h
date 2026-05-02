#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RiftEnemyBase.generated.h"

class ARiftRoomManager;
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
    void ConfigureEnemy(float NewMaxHealth, float NewMoveSpeed, float NewAttackDamage, float NewAttackCooldown, FVector NewVisualScale);

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

    UPROPERTY()
    TObjectPtr<ARiftRoomManager> OwningRoomManager;

    float LastAttackTime;
    float SpawnTime;

    APawn* FindTargetPawn() const;
    void TryAttack(APawn* TargetPawn);

    UFUNCTION()
    void HandleDeath(URiftHealthComponent* DeadHealthComponent, AActor* DamageInstigator);
};
