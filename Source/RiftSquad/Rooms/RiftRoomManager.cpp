#include "Rooms/RiftRoomManager.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Core/RiftGameState.h"
#include "Core/RiftPlayerController.h"
#include "Enemies/RiftEnemyBase.h"
#include "Items/RiftItemInventoryComponent.h"
#include "Net/UnrealNetwork.h"

ARiftRoomManager::ARiftRoomManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    bAutoStart = true;
    EnemyClass = ARiftEnemyBase::StaticClass();
    RoomPhase = ERiftRoomPhase::Idle;
    AliveEnemyCount = 0;
    CurrentRoomIndex = 0;

    SpawnOffsets.Add(FVector(620.0f, 0.0f, 80.0f));
    SpawnOffsets.Add(FVector(-620.0f, 320.0f, 80.0f));
    SpawnOffsets.Add(FVector(-620.0f, -320.0f, 80.0f));
    SpawnOffsets.Add(FVector(180.0f, 620.0f, 80.0f));
    SpawnOffsets.Add(FVector(180.0f, -620.0f, 80.0f));

    FRiftRoomWaveConfig RoomOne;
    RoomOne.EnemyCount = 2;
    RoomOne.EnemyHealth = 55.0f;
    RoomOne.EnemyMoveSpeed = 250.0f;
    RoomOne.EnemyAttackDamage = 3.0f;
    RoomOne.EnemyAttackCooldown = 2.0f;
    RoomOne.EnemyTypes = {ERiftEnemyType::Chaser, ERiftEnemyType::Chaser};
    RoomWaves.Add(RoomOne);

    FRiftRoomWaveConfig RoomTwo;
    RoomTwo.EnemyCount = 3;
    RoomTwo.EnemyHealth = 70.0f;
    RoomTwo.EnemyMoveSpeed = 285.0f;
    RoomTwo.EnemyAttackDamage = 4.0f;
    RoomTwo.EnemyTypes = {ERiftEnemyType::Chaser, ERiftEnemyType::Shooter, ERiftEnemyType::Chaser};
    RoomWaves.Add(RoomTwo);

    FRiftRoomWaveConfig RoomThree;
    RoomThree.EnemyCount = 4;
    RoomThree.EnemyHealth = 85.0f;
    RoomThree.EnemyMoveSpeed = 310.0f;
    RoomThree.EnemyAttackDamage = 5.0f;
    RoomThree.EnemyTypes = {ERiftEnemyType::Chaser, ERiftEnemyType::Shooter, ERiftEnemyType::Burster, ERiftEnemyType::Chaser};
    RoomWaves.Add(RoomThree);

    FRiftRoomWaveConfig EliteRoom;
    EliteRoom.EnemyCount = 2;
    EliteRoom.EnemyHealth = 135.0f;
    EliteRoom.EnemyMoveSpeed = 330.0f;
    EliteRoom.EnemyAttackDamage = 7.0f;
    EliteRoom.EnemyVisualScale = FVector(0.7f, 0.7f, 1.0f);
    EliteRoom.EnemyTypes = {ERiftEnemyType::Elite, ERiftEnemyType::Shooter};
    RoomWaves.Add(EliteRoom);

    FRiftRoomWaveConfig BossRoom;
    BossRoom.EnemyCount = 1;
    BossRoom.EnemyHealth = 420.0f;
    BossRoom.EnemyMoveSpeed = 260.0f;
    BossRoom.EnemyAttackDamage = 10.0f;
    BossRoom.EnemyAttackCooldown = 1.4f;
    BossRoom.EnemyVisualScale = FVector(1.35f, 1.35f, 1.8f);
    BossRoom.bBossRoom = true;
    BossRoom.EnemyTypes = {ERiftEnemyType::Elite};
    RoomWaves.Add(BossRoom);
}

void ARiftRoomManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority() && bAutoStart)
    {
        StartRun();
    }
}

void ARiftRoomManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARiftRoomManager, RoomPhase);
    DOREPLIFETIME(ARiftRoomManager, AliveEnemyCount);
    DOREPLIFETIME(ARiftRoomManager, CurrentRoomIndex);
}

void ARiftRoomManager::StartRun()
{
    if (!HasAuthority())
    {
        return;
    }

    for (ARiftEnemyBase* Enemy : ActiveEnemies)
    {
        if (IsValid(Enemy))
        {
            Enemy->Destroy();
        }
    }

    ActiveEnemies.Empty();
    CurrentRewardOptions.Empty();
    AliveEnemyCount = 0;
    CurrentRoomIndex = 0;
    RoomPhase = ERiftRoomPhase::Idle;
    UpdateGameState();
    StartRoom();
}

void ARiftRoomManager::StartRoom()
{
    if (!HasAuthority() || RoomPhase == ERiftRoomPhase::Combat || RoomWaves.Num() <= 0)
    {
        return;
    }

    if (CurrentRoomIndex >= RoomWaves.Num())
    {
        RoomPhase = ERiftRoomPhase::Completed;
        UpdateGameState();
        return;
    }

    CurrentRoomIndex++;
    RoomPhase = ERiftRoomPhase::Combat;
    ActiveEnemies.Empty();
    CurrentRewardOptions.Empty();

    SpawnCurrentRoomWave();

    AliveEnemyCount = ActiveEnemies.Num();
    if (AliveEnemyCount == 0)
    {
        CompleteRoom();
    }

    UpdateGameState();
}

void ARiftRoomManager::RegisterEnemy(ARiftEnemyBase* Enemy)
{
    if (!HasAuthority() || !Enemy)
    {
        return;
    }

    Enemy->SetOwningRoom(this);
    ActiveEnemies.AddUnique(Enemy);
    AliveEnemyCount = ActiveEnemies.Num();
    UpdateGameState();
}

void ARiftRoomManager::NotifyEnemyKilled(ARiftEnemyBase* Enemy)
{
    if (!HasAuthority() || !Enemy)
    {
        return;
    }

    ActiveEnemies.Remove(Enemy);
    AliveEnemyCount = ActiveEnemies.Num();

    if (AliveEnemyCount <= 0)
    {
        CompleteRoom();
    }
    else
    {
        UpdateGameState();
    }
}

void ARiftRoomManager::CompleteRoom()
{
    if (!HasAuthority())
    {
        return;
    }

    AliveEnemyCount = 0;

    const FRiftRoomWaveConfig* CompletedWave = RoomWaves.IsValidIndex(CurrentRoomIndex - 1) ? &RoomWaves[CurrentRoomIndex - 1] : nullptr;
    const bool bCompletedFinalRoom = CompletedWave ? CompletedWave->bBossRoom || CurrentRoomIndex >= RoomWaves.Num() : CurrentRoomIndex >= RoomWaves.Num();
    if (bCompletedFinalRoom)
    {
        RoomPhase = ERiftRoomPhase::Completed;
    }
    else
    {
        RoomPhase = ERiftRoomPhase::Reward;
        GenerateRewardOptions();
    }

    UpdateGameState();
}

void ARiftRoomManager::SelectRewardForPlayer(ARiftPlayerController* PlayerController, int32 OptionIndex)
{
    if (!HasAuthority() || RoomPhase != ERiftRoomPhase::Reward || !CurrentRewardOptions.IsValidIndex(OptionIndex))
    {
        return;
    }

    const FRiftRewardOption SelectedReward = CurrentRewardOptions[OptionIndex];
    ApplyRewardToPlayer(PlayerController, SelectedReward);
    CurrentRewardOptions.Empty();
    UpdateGameState();
    AdvanceToNextRoom();
}

void ARiftRoomManager::AdvanceToNextRoom()
{
    if (!HasAuthority() || RoomPhase != ERiftRoomPhase::Reward)
    {
        return;
    }

    StartRoom();
}

void ARiftRoomManager::SpawnCurrentRoomWave()
{
    UWorld* World = GetWorld();
    if (!World || !EnemyClass || !RoomWaves.IsValidIndex(CurrentRoomIndex - 1))
    {
        return;
    }

    const FRiftRoomWaveConfig& Wave = RoomWaves[CurrentRoomIndex - 1];
    const int32 SpawnCount = FMath::Max(0, Wave.EnemyTypes.Num() > 0 ? Wave.EnemyTypes.Num() : Wave.EnemyCount);
    for (int32 SpawnIndex = 0; SpawnIndex < SpawnCount; ++SpawnIndex)
    {
        const FVector SpawnLocation = GetSpawnLocationForIndex(SpawnIndex);
        ARiftEnemyBase* Enemy = World->SpawnActor<ARiftEnemyBase>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);
        if (Enemy)
        {
            const ERiftEnemyType EnemyType = Wave.EnemyTypes.IsValidIndex(SpawnIndex) ? Wave.EnemyTypes[SpawnIndex] : ERiftEnemyType::Chaser;
            Enemy->ConfigureEnemy(EnemyType, Wave.EnemyHealth, Wave.EnemyMoveSpeed, Wave.EnemyAttackDamage, Wave.EnemyAttackCooldown, Wave.EnemyVisualScale);
            RegisterEnemy(Enemy);
        }
    }
}

FVector ARiftRoomManager::GetSpawnLocationForIndex(int32 SpawnIndex) const
{
    if (SpawnOffsets.IsValidIndex(SpawnIndex))
    {
        return GetActorLocation() + SpawnOffsets[SpawnIndex];
    }

    const float Angle = (static_cast<float>(SpawnIndex) / FMath::Max(1.0f, static_cast<float>(SpawnOffsets.Num() + 1))) * 2.0f * PI;
    const float Radius = 420.0f;
    return GetActorLocation() + FVector(FMath::Cos(Angle) * Radius, FMath::Sin(Angle) * Radius, 80.0f);
}

void ARiftRoomManager::GenerateRewardOptions()
{
    CurrentRewardOptions.Empty();

    const int32 RewardSeed = FMath::Max(1, CurrentRoomIndex);
    TArray<FRiftRewardOption> RewardPool;
    auto MakeItem = [](FName ItemId, ERiftItemRarity Rarity, ERiftRewardType Type, float Magnitude, const TCHAR* Name, const TCHAR* Description)
    {
        FRiftRewardOption Option(Type, Magnitude, Name, Description);
        Option.ItemId = ItemId;
        Option.Rarity = Rarity;
        return Option;
    };

    RewardPool.Add(MakeItem(TEXT("ITEM_OverloadCapacitor"), ERiftItemRarity::Common, ERiftRewardType::WeaponDamage, 5.0f + RewardSeed, TEXT("Overload Capacitor"), TEXT("+ weapon damage")));
    RewardPool.Add(MakeItem(TEXT("ITEM_RapidCoil"), ERiftItemRarity::Common, ERiftRewardType::FireRate, 0.88f, TEXT("Rapid Coil"), TEXT("12% faster fire rate")));
    RewardPool.Add(MakeItem(TEXT("ITEM_RiftPlate"), ERiftItemRarity::Common, ERiftRewardType::MaxHealth, 25.0f, TEXT("Rift Plate"), TEXT("+ max HP and refill")));
    RewardPool.Add(MakeItem(TEXT("ITEM_FieldRepair"), ERiftItemRarity::Common, ERiftRewardType::Heal, 45.0f, TEXT("Field Repair"), TEXT("restore HP")));
    RewardPool.Add(MakeItem(TEXT("ITEM_LightBoots"), ERiftItemRarity::Common, ERiftRewardType::MoveSpeed, 1.10f, TEXT("Light Boots"), TEXT("+ movement speed")));
    RewardPool.Add(MakeItem(TEXT("ITEM_CooldownCore"), ERiftItemRarity::Rare, ERiftRewardType::SkillCooldown, 0.88f, TEXT("Cooldown Core"), TEXT("12% faster skill cooldowns")));
    RewardPool.Add(MakeItem(TEXT("ITEM_ShockAmplifier"), ERiftItemRarity::Rare, ERiftRewardType::ShockwaveDamage, 12.0f, TEXT("Shock Amplifier"), TEXT("+ shockwave damage")));
    RewardPool.Add(MakeItem(TEXT("ITEM_DiffusionModule"), ERiftItemRarity::Rare, ERiftRewardType::ShockwaveRadius, 1.18f, TEXT("Diffusion Module"), TEXT("+ shockwave radius")));
    RewardPool.Add(MakeItem(TEXT("ITEM_HeavyRounds"), ERiftItemRarity::Rare, ERiftRewardType::WeaponDamage, 10.0f, TEXT("Heavy Rounds"), TEXT("large weapon damage boost")));
    RewardPool.Add(MakeItem(TEXT("ITEM_CombatInjector"), ERiftItemRarity::Rare, ERiftRewardType::MoveSpeed, 1.18f, TEXT("Combat Injector"), TEXT("large movement speed boost")));
    RewardPool.Add(MakeItem(TEXT("ITEM_RiftBattery"), ERiftItemRarity::Legendary, ERiftRewardType::FireRate, 0.72f, TEXT("Rift Battery"), TEXT("massive fire rate boost")));
    RewardPool.Add(MakeItem(TEXT("ITEM_CoreResonator"), ERiftItemRarity::Legendary, ERiftRewardType::ShockwaveDamage, 28.0f, TEXT("Core Resonator"), TEXT("massive shockwave damage")));

    for (int32 OptionIndex = 0; OptionIndex < 3 && RewardPool.Num() > 0; ++OptionIndex)
    {
        const int32 PoolIndex = (RewardSeed + OptionIndex * 2) % RewardPool.Num();
        CurrentRewardOptions.Add(RewardPool[PoolIndex]);
        RewardPool.RemoveAt(PoolIndex);
    }
}

void ARiftRoomManager::ApplyRewardToPlayer(ARiftPlayerController* PlayerController, const FRiftRewardOption& RewardOption)
{
    ARiftPlayerCharacter* PlayerCharacter = PlayerController ? Cast<ARiftPlayerCharacter>(PlayerController->GetPawn()) : nullptr;
    if (!PlayerCharacter)
    {
        return;
    }

    if (URiftItemInventoryComponent* InventoryComponent = PlayerCharacter->GetItemInventoryComponent())
    {
        InventoryComponent->ApplyRewardOption(RewardOption);
    }

    if (ARiftGameState* RiftGameState = GetWorld() ? GetWorld()->GetGameState<ARiftGameState>() : nullptr)
    {
        const FString Summary = FString::Printf(TEXT("Selected: %s"), *RewardOption.Name);
        RiftGameState->SetLastRewardSummary(Summary);
    }
}

void ARiftRoomManager::UpdateGameState()
{
    if (ARiftGameState* RiftGameState = GetWorld() ? GetWorld()->GetGameState<ARiftGameState>() : nullptr)
    {
        RiftGameState->SetCurrentRoomPhase(RoomPhase);
        RiftGameState->SetAliveEnemyCount(AliveEnemyCount);
        RiftGameState->SetRoomProgress(CurrentRoomIndex, RoomWaves.Num());
        RiftGameState->SetRewardOptions(CurrentRewardOptions);

        ERiftRunPhase RunPhase = ERiftRunPhase::Setup;
        switch (RoomPhase)
        {
            case ERiftRoomPhase::Combat:
                RunPhase = ERiftRunPhase::Combat;
                break;
            case ERiftRoomPhase::Reward:
                RunPhase = ERiftRunPhase::Reward;
                break;
            case ERiftRoomPhase::Completed:
                RunPhase = ERiftRunPhase::Victory;
                break;
            default:
                RunPhase = ERiftRunPhase::Setup;
                break;
        }

        RiftGameState->SetCurrentRunPhase(RunPhase);
    }
}

void ARiftRoomManager::OnRep_RoomPhase()
{
}

void ARiftRoomManager::OnRep_AliveEnemyCount()
{
}

void ARiftRoomManager::OnRep_CurrentRoomIndex()
{
}
