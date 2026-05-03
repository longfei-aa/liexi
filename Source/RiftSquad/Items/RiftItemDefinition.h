#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "Engine/DataAsset.h"
#include "RiftItemDefinition.generated.h"

UCLASS(BlueprintType)
class RIFTSQUAD_API URiftItemDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Item")
    FName ItemId;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Item")
    FString DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Item")
    FString Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Item")
    ERiftItemRarity Rarity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Item")
    ERiftRewardType ModifierType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rift|Item")
    float Magnitude;
};
