#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RiftGameMode.generated.h"

class ARiftRoomManager;

UCLASS()
class RIFTSQUAD_API ARiftGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ARiftGameMode();

    virtual void BeginPlay() override;

    void NotifyPlayerDied();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Room")
    bool bSpawnInitialRoomManager;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Room")
    TSubclassOf<ARiftRoomManager> RoomManagerClass;

    bool AreAllPlayersDead() const;
};
