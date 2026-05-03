#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "UObject/Object.h"
#include "RiftItemLibrary.generated.h"

UCLASS()
class RIFTSQUAD_API URiftItemLibrary : public UObject
{
    GENERATED_BODY()

public:
    static TArray<FRiftRewardOption> BuildDefaultItemPool();
    static TArray<FRiftRewardOption> GenerateRewardOptions(int32 RoomIndex, bool bEliteReward, int32 OptionCount);

protected:
    static ERiftItemRarity PickTargetRarity(int32 RoomIndex, bool bEliteReward, int32 OptionIndex);
};
