#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "GameFramework/GameStateBase.h"
#include "RiftGameState.generated.h"

UCLASS()
class RIFTSQUAD_API ARiftGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ARiftGameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure, Category = "Rift|Room")
    ERiftRoomPhase GetCurrentRoomPhase() const { return CurrentRoomPhase; }

    UFUNCTION(BlueprintPure, Category = "Rift|Room")
    int32 GetAliveEnemyCount() const { return AliveEnemyCount; }

    UFUNCTION(BlueprintPure, Category = "Rift|Run")
    ERiftRunPhase GetCurrentRunPhase() const { return CurrentRunPhase; }

    UFUNCTION(BlueprintPure, Category = "Rift|Run")
    int32 GetCurrentRoomIndex() const { return CurrentRoomIndex; }

    UFUNCTION(BlueprintPure, Category = "Rift|Run")
    int32 GetTotalRoomCount() const { return TotalRoomCount; }

    UFUNCTION(BlueprintPure, Category = "Rift|Reward")
    const TArray<FRiftRewardOption>& GetRewardOptions() const { return RewardOptions; }

    UFUNCTION(BlueprintPure, Category = "Rift|Reward")
    FString GetLastRewardSummary() const { return LastRewardSummary; }

    void SetCurrentRoomPhase(ERiftRoomPhase NewPhase);
    void SetAliveEnemyCount(int32 NewCount);
    void SetCurrentRunPhase(ERiftRunPhase NewPhase);
    void SetRoomProgress(int32 NewCurrentRoomIndex, int32 NewTotalRoomCount);
    void SetRewardOptions(const TArray<FRiftRewardOption>& NewRewardOptions);
    void SetLastRewardSummary(const FString& NewSummary);

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentRunPhase, BlueprintReadOnly, Category = "Rift|Run")
    ERiftRunPhase CurrentRunPhase;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentRoomPhase, BlueprintReadOnly, Category = "Rift|Room")
    ERiftRoomPhase CurrentRoomPhase;

    UPROPERTY(ReplicatedUsing = OnRep_AliveEnemyCount, BlueprintReadOnly, Category = "Rift|Room")
    int32 AliveEnemyCount;

    UPROPERTY(ReplicatedUsing = OnRep_RoomProgress, BlueprintReadOnly, Category = "Rift|Run")
    int32 CurrentRoomIndex;

    UPROPERTY(ReplicatedUsing = OnRep_RoomProgress, BlueprintReadOnly, Category = "Rift|Run")
    int32 TotalRoomCount;

    UPROPERTY(ReplicatedUsing = OnRep_RewardOptions, BlueprintReadOnly, Category = "Rift|Reward")
    TArray<FRiftRewardOption> RewardOptions;

    UPROPERTY(ReplicatedUsing = OnRep_LastRewardSummary, BlueprintReadOnly, Category = "Rift|Reward")
    FString LastRewardSummary;

    UFUNCTION()
    void OnRep_CurrentRunPhase();

    UFUNCTION()
    void OnRep_CurrentRoomPhase();

    UFUNCTION()
    void OnRep_AliveEnemyCount();

    UFUNCTION()
    void OnRep_RoomProgress();

    UFUNCTION()
    void OnRep_RewardOptions();

    UFUNCTION()
    void OnRep_LastRewardSummary();
};
