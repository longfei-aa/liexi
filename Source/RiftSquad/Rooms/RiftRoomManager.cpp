#include "Rooms/RiftRoomManager.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Combat/RiftHealthComponent.h"
#include "Combat/RiftWeaponComponent.h"
#include "Core/RiftGameState.h"
#include "Core/RiftPlayerController.h"
#include "Enemies/RiftEnemyBase.h"
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

    SpawnOffsets.Add(FVector(300.0f, 0.0f, 80.0f));
    SpawnOffsets.Add(FVector(-300.0f, 200.0f, 80.0f));
    SpawnOffsets.Add(FVector(-300.0f, -200.0f, 80.0f));

    FRiftRoomWaveConfig RoomOne;
    RoomOne.EnemyCount = 3;
    RoomOne.EnemyHealth = 75.0f;
    RoomWaves.Add(RoomOne);

    FRiftRoomWaveConfig RoomTwo;
    RoomTwo.EnemyCount = 4;
    RoomTwo.EnemyHealth = 90.0f;
    RoomWaves.Add(RoomTwo);

    FRiftRoomWaveConfig RoomThree;
    RoomThree.EnemyCount = 5;
    RoomThree.EnemyHealth = 105.0f;
    RoomThree.EnemyAttackDamage = 10.0f;
    RoomWaves.Add(RoomThree);

    FRiftRoomWaveConfig EliteRoom;
    EliteRoom.EnemyCount = 3;
    EliteRoom.EnemyHealth = 170.0f;
    EliteRoom.EnemyMoveSpeed = 410.0f;
    EliteRoom.EnemyAttackDamage = 14.0f;
    EliteRoom.EnemyVisualScale = FVector(0.7f, 0.7f, 1.0f);
    RoomWaves.Add(EliteRoom);

    FRiftRoomWaveConfig BossRoom;
    BossRoom.EnemyCount = 1;
    BossRoom.EnemyHealth = 650.0f;
    BossRoom.EnemyMoveSpeed = 300.0f;
    BossRoom.EnemyAttackDamage = 22.0f;
    BossRoom.EnemyAttackCooldown = 1.0f;
    BossRoom.EnemyVisualScale = FVector(1.35f, 1.35f, 1.8f);
    BossRoom.bBossRoom = true;
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
    const int32 SpawnCount = FMath::Max(0, Wave.EnemyCount);
    for (int32 SpawnIndex = 0; SpawnIndex < SpawnCount; ++SpawnIndex)
    {
        const FVector SpawnLocation = GetSpawnLocationForIndex(SpawnIndex);
        ARiftEnemyBase* Enemy = World->SpawnActor<ARiftEnemyBase>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);
        if (Enemy)
        {
            Enemy->ConfigureEnemy(Wave.EnemyHealth, Wave.EnemyMoveSpeed, Wave.EnemyAttackDamage, Wave.EnemyAttackCooldown, Wave.EnemyVisualScale);
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
    RewardPool.Add(FRiftRewardOption(ERiftRewardType::WeaponDamage, 4.0f + RewardSeed, TEXT("Overcharged Rounds"), TEXT("+ weapon damage")));
    RewardPool.Add(FRiftRewardOption(ERiftRewardType::FireRate, 0.85f, TEXT("Rapid Capacitor"), TEXT("15% faster fire rate")));
    RewardPool.Add(FRiftRewardOption(ERiftRewardType::MaxHealth, 25.0f, TEXT("Alloy Plating"), TEXT("+ max HP and refill")));
    RewardPool.Add(FRiftRewardOption(ERiftRewardType::Heal, 35.0f, TEXT("Field Repair"), TEXT("restore HP")));
    RewardPool.Add(FRiftRewardOption(ERiftRewardType::MoveSpeed, 1.12f, TEXT("Phase Boots"), TEXT("+ movement speed")));

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

    URiftHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();
    URiftWeaponComponent* WeaponComponent = PlayerCharacter->GetWeaponComponent();

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
            PlayerCharacter->MultiplyMoveSpeed(RewardOption.Magnitude);
            break;
        default:
            break;
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
