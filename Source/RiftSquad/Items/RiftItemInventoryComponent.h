#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/RiftTypes.h"
#include "RiftItemInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FRiftCollectedItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Item")
    FName ItemId;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Item")
    FString DisplayName;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Item")
    ERiftItemRarity Rarity = ERiftItemRarity::Common;
};

UCLASS(ClassGroup = (Rift), meta = (BlueprintSpawnableComponent))
class RIFTSQUAD_API URiftItemInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URiftItemInventoryComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Rift|Item")
    void ApplyRewardOption(const FRiftRewardOption& RewardOption);

    UFUNCTION(BlueprintPure, Category = "Rift|Item")
    const TArray<FRiftCollectedItem>& GetCollectedItems() const { return CollectedItems; }

    UFUNCTION(BlueprintPure, Category = "Rift|Item")
    FString GetRecentItemSummary(int32 MaxItems = 3) const;

protected:
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Rift|Item")
    TArray<FRiftCollectedItem> CollectedItems;

    void AddCollectedItem(const FRiftRewardOption& RewardOption);
};
