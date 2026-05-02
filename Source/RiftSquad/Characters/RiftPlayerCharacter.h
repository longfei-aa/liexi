#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RiftPlayerCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UStaticMeshComponent;
class USpringArmComponent;
class URiftHealthComponent;
class URiftWeaponComponent;
struct FInputActionValue;

UCLASS()
class RIFTSQUAD_API ARiftPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARiftPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category = "Rift|Components")
    URiftHealthComponent* GetHealthComponent() const { return HealthComponent; }

    UFUNCTION(BlueprintPure, Category = "Rift|Components")
    URiftWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

    UFUNCTION(BlueprintCallable, Category = "Rift|Stats")
    void MultiplyMoveSpeed(float Multiplier);

    UFUNCTION(BlueprintPure, Category = "Rift|Stats")
    float GetCurrentMoveSpeed() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Camera")
    TObjectPtr<UCameraComponent> TopDownCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftWeaponComponent> WeaponComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Visual")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Input")
    TObjectPtr<UInputAction> FireAction;

    void Move(const FInputActionValue& Value);
    void Fire(const FInputActionValue& Value);
    void MoveForward(float Value);
    void MoveRight(float Value);
    void FireLegacy();
    FVector GetAimDirection() const;

    UFUNCTION()
    void HandleDeath(URiftHealthComponent* DeadHealthComponent, AActor* DamageInstigator);
};
