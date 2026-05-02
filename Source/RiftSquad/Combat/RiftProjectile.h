#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiftProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class RIFTSQUAD_API ARiftProjectile : public AActor
{
    GENERATED_BODY()

public:
    ARiftProjectile();

    virtual void BeginPlay() override;

    void InitializeProjectile(AActor* InInstigatorActor, float InDamage);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Projectile")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Projectile")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Projectile")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Projectile")
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Projectile")
    float LifeSeconds;

    UPROPERTY()
    TObjectPtr<AActor> InstigatorActor;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
