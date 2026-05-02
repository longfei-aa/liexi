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

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Room")
    bool bSpawnInitialRoomManager;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Debug")
    bool bSpawnRuntimeVisualFallback;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Room")
    TSubclassOf<ARiftRoomManager> RoomManagerClass;

    void SpawnRuntimeVisualFallback();
};
