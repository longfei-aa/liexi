#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RiftPlayerCharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UStaticMeshComponent;
class USpringArmComponent;
class URiftAbilityComponent;
class URiftHealthComponent;
class URiftItemInventoryComponent;
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

    UFUNCTION(BlueprintPure, Category = "Rift|Components")
    URiftAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }

    UFUNCTION(BlueprintPure, Category = "Rift|Components")
    URiftItemInventoryComponent* GetItemInventoryComponent() const { return ItemInventoryComponent; }

    UFUNCTION(BlueprintCallable, Category = "Rift|Stats")
    void MultiplyMoveSpeed(float Multiplier);

    UFUNCTION(BlueprintPure, Category = "Rift|Stats")
    float GetCurrentMoveSpeed() const;

    UFUNCTION(BlueprintCallable, Category = "Rift|Run")
    void ResetForNewRun(const FVector& SpawnLocation);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Camera")
    TObjectPtr<UCameraComponent> TopDownCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftWeaponComponent> WeaponComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftAbilityComponent> AbilityComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rift|Components")
    TObjectPtr<URiftItemInventoryComponent> ItemInventoryComponent;

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
    void UseAbilityLegacy();
    void DashLegacy();
    bool CanUseGameplayInput() const;
    FVector GetAimDirection() const;

    UFUNCTION()
    void HandleDeath(URiftHealthComponent* DeadHealthComponent, AActor* DamageInstigator);
};
