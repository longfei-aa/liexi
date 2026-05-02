#pragma once

#include "CoreMinimal.h"
#include "RiftTypes.generated.h"

UENUM(BlueprintType)
enum class ERiftRoomPhase : uint8
{
    Idle,
    Loading,
    Intro,
    Combat,
    Reward,
    ExitReady,
    Completed
};
