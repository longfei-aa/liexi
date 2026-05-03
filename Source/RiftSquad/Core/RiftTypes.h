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
    Supply,
    ExitReady,
    Completed
};

UENUM(BlueprintType)
enum class ERiftRunPhase : uint8
{
    Setup,
    Combat,
    Reward,
    Supply,
    Victory,
    Defeat
};

UENUM(BlueprintType)
enum class ERiftRewardType : uint8
{
    None,
    WeaponDamage,
    FireRate,
    MaxHealth,
    Heal,
    MoveSpeed,
    SkillCooldown,
    ShockwaveDamage,
    ShockwaveRadius
};

UENUM(BlueprintType)
enum class ERiftItemRarity : uint8
{
    Common,
    Rare,
    Legendary
};

UENUM(BlueprintType)
enum class ERiftEnemyType : uint8
{
    Chaser,
    Shooter,
    Burster,
    Elite
};

USTRUCT(BlueprintType)
struct FRiftRewardOption
{
    GENERATED_BODY()

    FRiftRewardOption()
        : Type(ERiftRewardType::None)
        , ItemId(NAME_None)
        , Rarity(ERiftItemRarity::Common)
        , Magnitude(0.0f)
    {
    }

    FRiftRewardOption(ERiftRewardType InType, float InMagnitude, const FString& InName, const FString& InDescription)
        : Type(InType)
        , ItemId(NAME_None)
        , Rarity(ERiftItemRarity::Common)
        , Magnitude(InMagnitude)
        , Name(InName)
        , Description(InDescription)
    {
    }

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    ERiftRewardType Type;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    FName ItemId;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    ERiftItemRarity Rarity;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    float Magnitude;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    FString Description;
};
