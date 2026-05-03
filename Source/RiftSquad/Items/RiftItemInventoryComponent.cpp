#include "Items/RiftItemInventoryComponent.h"

#include "Abilities/RiftAbilityComponent.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Combat/RiftHealthComponent.h"
#include "Combat/RiftWeaponComponent.h"
#include "Net/UnrealNetwork.h"

URiftItemInventoryComponent::URiftItemInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void URiftItemInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URiftItemInventoryComponent, CollectedItems);
}

void URiftItemInventoryComponent::ApplyRewardOption(const FRiftRewardOption& RewardOption)
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority() || RewardOption.Type == ERiftRewardType::None)
    {
        return;
    }

    ARiftPlayerCharacter* PlayerCharacter = Cast<ARiftPlayerCharacter>(Owner);
    URiftHealthComponent* HealthComponent = Owner->FindComponentByClass<URiftHealthComponent>();
    URiftWeaponComponent* WeaponComponent = Owner->FindComponentByClass<URiftWeaponComponent>();
    URiftAbilityComponent* AbilityComponent = Owner->FindComponentByClass<URiftAbilityComponent>();

    switch (RewardOption.Type)
    {
        case ERiftRewardType::WeaponDamage:
            if (WeaponComponent)
            {
                WeaponComponent->AddDamage(RewardOption.Magnitude);
            }
            break;
        case ERiftRewardType::FireRate:
            if (WeaponComponent)
            {
                WeaponComponent->MultiplyFireInterval(RewardOption.Magnitude);
            }
            break;
        case ERiftRewardType::MaxHealth:
            if (HealthComponent)
            {
                HealthComponent->SetMaxHealth(HealthComponent->GetMaxHealth() + RewardOption.Magnitude, true);
            }
            break;
        case ERiftRewardType::Heal:
            if (HealthComponent)
            {
                HealthComponent->Heal(RewardOption.Magnitude);
            }
            break;
        case ERiftRewardType::MoveSpeed:
            if (PlayerCharacter)
            {
                PlayerCharacter->MultiplyMoveSpeed(RewardOption.Magnitude);
            }
            break;
        case ERiftRewardType::SkillCooldown:
            if (AbilityComponent)
            {
                AbilityComponent->MultiplyCooldowns(RewardOption.Magnitude);
            }
            break;
        case ERiftRewardType::ShockwaveDamage:
            if (AbilityComponent)
            {
                AbilityComponent->AddShockwaveDamage(RewardOption.Magnitude);
            }
            break;
        case ERiftRewardType::ShockwaveRadius:
            if (AbilityComponent)
            {
                AbilityComponent->MultiplyShockwaveRadius(RewardOption.Magnitude);
            }
            break;
        default:
            break;
    }

    AddCollectedItem(RewardOption);
}

FString URiftItemInventoryComponent::GetRecentItemSummary(int32 MaxItems) const
{
    if (CollectedItems.Num() == 0)
    {
        return TEXT("Items: none");
    }

    TArray<FString> Names;
    const int32 StartIndex = FMath::Max(0, CollectedItems.Num() - FMath::Max(1, MaxItems));
    for (int32 Index = StartIndex; Index < CollectedItems.Num(); ++Index)
    {
        Names.Add(CollectedItems[Index].DisplayName);
    }

    return FString::Printf(TEXT("Items: %s"), *FString::Join(Names, TEXT(", ")));
}

void URiftItemInventoryComponent::ResetInventory()
{
    AActor* Owner = GetOwner();
    if (!Owner || !Owner->HasAuthority())
    {
        return;
    }

    CollectedItems.Empty();
}

void URiftItemInventoryComponent::AddCollectedItem(const FRiftRewardOption& RewardOption)
{
    FRiftCollectedItem CollectedItem;
    CollectedItem.ItemId = RewardOption.ItemId;
    CollectedItem.DisplayName = RewardOption.Name;
    CollectedItem.Rarity = RewardOption.Rarity;
    CollectedItems.Add(CollectedItem);
}
