#include "Items/RiftItemLibrary.h"

TArray<FRiftRewardOption> URiftItemLibrary::BuildDefaultItemPool()
{
    auto MakeItem = [](FName ItemId, ERiftItemRarity Rarity, ERiftRewardType Type, float Magnitude, const TCHAR* Name, const TCHAR* Description)
    {
        FRiftRewardOption Option(Type, Magnitude, Name, Description);
        Option.ItemId = ItemId;
        Option.Rarity = Rarity;
        return Option;
    };

    TArray<FRiftRewardOption> Pool;
    Pool.Add(MakeItem(TEXT("ITEM_OverloadCapacitor"), ERiftItemRarity::Common, ERiftRewardType::WeaponDamage, 6.0f, TEXT("Overload Capacitor"), TEXT("+ weapon damage")));
    Pool.Add(MakeItem(TEXT("ITEM_RapidCoil"), ERiftItemRarity::Common, ERiftRewardType::FireRate, 0.88f, TEXT("Rapid Coil"), TEXT("12% faster fire rate")));
    Pool.Add(MakeItem(TEXT("ITEM_RiftPlate"), ERiftItemRarity::Common, ERiftRewardType::MaxHealth, 25.0f, TEXT("Rift Plate"), TEXT("+ max HP and refill")));
    Pool.Add(MakeItem(TEXT("ITEM_FieldRepair"), ERiftItemRarity::Common, ERiftRewardType::Heal, 45.0f, TEXT("Field Repair"), TEXT("restore HP")));
    Pool.Add(MakeItem(TEXT("ITEM_LightBoots"), ERiftItemRarity::Common, ERiftRewardType::MoveSpeed, 1.10f, TEXT("Light Boots"), TEXT("+ movement speed")));
    Pool.Add(MakeItem(TEXT("ITEM_CooldownCore"), ERiftItemRarity::Rare, ERiftRewardType::SkillCooldown, 0.88f, TEXT("Cooldown Core"), TEXT("12% faster skill cooldowns")));
    Pool.Add(MakeItem(TEXT("ITEM_ShockAmplifier"), ERiftItemRarity::Rare, ERiftRewardType::ShockwaveDamage, 12.0f, TEXT("Shock Amplifier"), TEXT("+ shockwave damage")));
    Pool.Add(MakeItem(TEXT("ITEM_DiffusionModule"), ERiftItemRarity::Rare, ERiftRewardType::ShockwaveRadius, 1.18f, TEXT("Diffusion Module"), TEXT("+ shockwave radius")));
    Pool.Add(MakeItem(TEXT("ITEM_HeavyRounds"), ERiftItemRarity::Rare, ERiftRewardType::WeaponDamage, 10.0f, TEXT("Heavy Rounds"), TEXT("large weapon damage boost")));
    Pool.Add(MakeItem(TEXT("ITEM_CombatInjector"), ERiftItemRarity::Rare, ERiftRewardType::MoveSpeed, 1.18f, TEXT("Combat Injector"), TEXT("large movement speed boost")));
    Pool.Add(MakeItem(TEXT("ITEM_RiftBattery"), ERiftItemRarity::Legendary, ERiftRewardType::FireRate, 0.72f, TEXT("Rift Battery"), TEXT("massive fire rate boost")));
    Pool.Add(MakeItem(TEXT("ITEM_CoreResonator"), ERiftItemRarity::Legendary, ERiftRewardType::ShockwaveDamage, 28.0f, TEXT("Core Resonator"), TEXT("massive shockwave damage")));
    return Pool;
}

TArray<FRiftRewardOption> URiftItemLibrary::GenerateRewardOptions(int32 RoomIndex, bool bEliteReward, int32 OptionCount)
{
    TArray<FRiftRewardOption> Pool = BuildDefaultItemPool();
    TArray<FRiftRewardOption> Options;

    const int32 SafeOptionCount = FMath::Max(1, OptionCount);
    const int32 RewardSeed = FMath::Max(1, RoomIndex);
    for (int32 OptionIndex = 0; OptionIndex < SafeOptionCount && Pool.Num() > 0; ++OptionIndex)
    {
        const ERiftItemRarity TargetRarity = PickTargetRarity(RewardSeed, bEliteReward, OptionIndex);

        TArray<int32> CandidateIndexes;
        for (int32 PoolIndex = 0; PoolIndex < Pool.Num(); ++PoolIndex)
        {
            if (Pool[PoolIndex].Rarity == TargetRarity)
            {
                CandidateIndexes.Add(PoolIndex);
            }
        }

        if (CandidateIndexes.Num() == 0)
        {
            for (int32 PoolIndex = 0; PoolIndex < Pool.Num(); ++PoolIndex)
            {
                CandidateIndexes.Add(PoolIndex);
            }
        }

        const int32 CandidateOffset = (RewardSeed + OptionIndex * 3) % CandidateIndexes.Num();
        const int32 SelectedPoolIndex = CandidateIndexes[CandidateOffset];
        Options.Add(Pool[SelectedPoolIndex]);
        Pool.RemoveAt(SelectedPoolIndex);
    }

    return Options;
}

TArray<FRiftRewardOption> URiftItemLibrary::GenerateSupplyOptions(int32 RoomIndex, int32 OptionCount)
{
    TArray<FRiftRewardOption> Pool = BuildDefaultItemPool();
    Pool = Pool.FilterByPredicate([](const FRiftRewardOption& Option)
    {
        return Option.Type == ERiftRewardType::Heal ||
            Option.Type == ERiftRewardType::MaxHealth ||
            Option.Type == ERiftRewardType::SkillCooldown ||
            Option.Type == ERiftRewardType::MoveSpeed;
    });

    TArray<FRiftRewardOption> Options;
    const int32 RewardSeed = FMath::Max(1, RoomIndex);
    const int32 SafeOptionCount = FMath::Max(1, OptionCount);
    for (int32 OptionIndex = 0; OptionIndex < SafeOptionCount && Pool.Num() > 0; ++OptionIndex)
    {
        const int32 PoolIndex = (RewardSeed + OptionIndex * 2) % Pool.Num();
        Options.Add(Pool[PoolIndex]);
        Pool.RemoveAt(PoolIndex);
    }

    return Options;
}

ERiftItemRarity URiftItemLibrary::PickTargetRarity(int32 RoomIndex, bool bEliteReward, int32 OptionIndex)
{
    const int32 Roll = (RoomIndex * 37 + OptionIndex * 23) % 100;

    if (bEliteReward)
    {
        if (Roll < 10)
        {
            return ERiftItemRarity::Legendary;
        }
        if (Roll < 65)
        {
            return ERiftItemRarity::Rare;
        }
        return ERiftItemRarity::Common;
    }

    if (Roll < 3)
    {
        return ERiftItemRarity::Legendary;
    }
    if (Roll < 30)
    {
        return ERiftItemRarity::Rare;
    }
    return ERiftItemRarity::Common;
}
