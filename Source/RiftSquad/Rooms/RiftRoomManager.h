#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "GameFramework/Actor.h"
#include "RiftRoomManager.generated.h"

class ARiftEnemyBase;

USTRUCT(BlueprintType)
struct FRiftRoomWaveConfig
{
    GENERATED_BODY()

    FRiftRoomWaveConfig()
        : EnemyCount(3)
        , EnemyHealth(100.0f)
        , EnemyMoveSpeed(360.0f)
        , EnemyAttackDamage(8.0f)
        , EnemyAttackCooldown(1.25f)
        , EnemyVisualScale(0.5f, 0.5f, 0.8f)
        , bBossRoom(false)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    int32 EnemyCount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    float EnemyHealth;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    float EnemyMoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    float EnemyAttackDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    float EnemyAttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    FVector EnemyVisualScale;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    bool bBossRoom;
};

UCLASS()
class RIFTSQUAD_API ARiftRoomManager : public AActor
{
    GENERATED_BODY()

public:
    ARiftRoomManager();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Rift|Room")
    void StartRun();

    UFUNCTION(BlueprintCallable, Category = "Rift|Room")
    void StartRoom();

    UFUNCTION(BlueprintCallable, Category = "Rift|Room")
    void RegisterEnemy(ARiftEnemyBase* Enemy);

    UFUNCTION(BlueprintCallable, Category = "Rift|Room")
    void NotifyEnemyKilled(ARiftEnemyBase* Enemy);

    UFUNCTION(BlueprintPure, Category = "Rift|Room")
    int32 GetAliveEnemyCount() const { return AliveEnemyCount; }

    UFUNCTION(BlueprintPure, Category = "Rift|Room")
    ERiftRoomPhase GetRoomPhase() const { return RoomPhase; }

    UFUNCTION(BlueprintPure, Category = "Rift|Run")
    int32 GetCurrentRoomIndex() const { return CurrentRoomIndex; }

    UFUNCTION(BlueprintPure, Category = "Rift|Run")
    int32 GetTotalRoomCount() const { return RoomWaves.Num(); }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    bool bAutoStart;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    TSubclassOf<ARiftEnemyBase> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    TArray<FVector> SpawnOffsets;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    TArray<FRiftRoomWaveConfig> RoomWaves;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    float RewardPhaseDuration;

    UPROPERTY(ReplicatedUsing = OnRep_RoomPhase, BlueprintReadOnly, Category = "Rift|Room")
    ERiftRoomPhase RoomPhase;

    UPROPERTY(ReplicatedUsing = OnRep_AliveEnemyCount, BlueprintReadOnly, Category = "Rift|Room")
    int32 AliveEnemyCount;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentRoomIndex, BlueprintReadOnly, Category = "Rift|Run")
    int32 CurrentRoomIndex;

    UPROPERTY()
    TArray<TObjectPtr<ARiftEnemyBase>> ActiveEnemies;

    FTimerHandle RewardAdvanceTimerHandle;

    UFUNCTION()
    void OnRep_RoomPhase();

    UFUNCTION()
    void OnRep_AliveEnemyCount();

    UFUNCTION()
    void OnRep_CurrentRoomIndex();

    void CompleteRoom();
    void AdvanceToNextRoom();
    void SpawnCurrentRoomWave();
    FVector GetSpawnLocationForIndex(int32 SpawnIndex) const;
    void UpdateGameState();
};
