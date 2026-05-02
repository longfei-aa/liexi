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

    void SetCurrentRoomPhase(ERiftRoomPhase NewPhase);
    void SetAliveEnemyCount(int32 NewCount);

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentRoomPhase, BlueprintReadOnly, Category = "Rift|Room")
    ERiftRoomPhase CurrentRoomPhase;

    UPROPERTY(ReplicatedUsing = OnRep_AliveEnemyCount, BlueprintReadOnly, Category = "Rift|Room")
    int32 AliveEnemyCount;

    UFUNCTION()
    void OnRep_CurrentRoomPhase();

    UFUNCTION()
    void OnRep_AliveEnemyCount();
};
