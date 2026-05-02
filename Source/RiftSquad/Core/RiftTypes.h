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

UENUM(BlueprintType)
enum class ERiftRunPhase : uint8
{
    Setup,
    Combat,
    Reward,
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
    MoveSpeed
};

USTRUCT(BlueprintType)
struct FRiftRewardOption
{
    GENERATED_BODY()

    FRiftRewardOption()
        : Type(ERiftRewardType::None)
        , Magnitude(0.0f)
    {
    }

    FRiftRewardOption(ERiftRewardType InType, float InMagnitude, const FString& InName, const FString& InDescription)
        : Type(InType)
        , Magnitude(InMagnitude)
        , Name(InName)
        , Description(InDescription)
    {
    }

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    ERiftRewardType Type;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    float Magnitude;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    FString Name;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Reward")
    FString Description;
};
