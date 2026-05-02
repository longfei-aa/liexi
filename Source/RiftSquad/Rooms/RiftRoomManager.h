#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "GameFramework/Actor.h"
#include "RiftRoomManager.generated.h"

class ARiftEnemyBase;

UCLASS()
class RIFTSQUAD_API ARiftRoomManager : public AActor
{
    GENERATED_BODY()

public:
    ARiftRoomManager();

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    bool bAutoStart;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    TSubclassOf<ARiftEnemyBase> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rift|Room")
    TArray<FVector> SpawnOffsets;

    UPROPERTY(ReplicatedUsing = OnRep_RoomPhase, BlueprintReadOnly, Category = "Rift|Room")
    ERiftRoomPhase RoomPhase;

    UPROPERTY(ReplicatedUsing = OnRep_AliveEnemyCount, BlueprintReadOnly, Category = "Rift|Room")
    int32 AliveEnemyCount;

    UPROPERTY()
    TArray<TObjectPtr<ARiftEnemyBase>> ActiveEnemies;

    UFUNCTION()
    void OnRep_RoomPhase();

    UFUNCTION()
    void OnRep_AliveEnemyCount();

    void CompleteRoom();
    void UpdateGameState();
};
