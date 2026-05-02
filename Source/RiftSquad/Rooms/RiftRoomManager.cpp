#include "Rooms/RiftRoomManager.h"
#include "Core/RiftGameState.h"
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

    SpawnOffsets.Add(FVector(300.0f, 0.0f, 80.0f));
    SpawnOffsets.Add(FVector(-300.0f, 200.0f, 80.0f));
    SpawnOffsets.Add(FVector(-300.0f, -200.0f, 80.0f));
}

void ARiftRoomManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority() && bAutoStart)
    {
        StartRoom();
    }
}

void ARiftRoomManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARiftRoomManager, RoomPhase);
    DOREPLIFETIME(ARiftRoomManager, AliveEnemyCount);
}

void ARiftRoomManager::StartRoom()
{
    if (!HasAuthority() || RoomPhase == ERiftRoomPhase::Combat)
    {
        return;
    }

    RoomPhase = ERiftRoomPhase::Combat;
    ActiveEnemies.Empty();

    UWorld* World = GetWorld();
    if (World && EnemyClass)
    {
        for (const FVector& SpawnOffset : SpawnOffsets)
        {
            const FVector SpawnLocation = GetActorLocation() + SpawnOffset;
            ARiftEnemyBase* Enemy = World->SpawnActor<ARiftEnemyBase>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);
            RegisterEnemy(Enemy);
        }
    }

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

    RoomPhase = ERiftRoomPhase::Completed;
    AliveEnemyCount = 0;
    UpdateGameState();
}

void ARiftRoomManager::UpdateGameState()
{
    if (ARiftGameState* RiftGameState = GetWorld() ? GetWorld()->GetGameState<ARiftGameState>() : nullptr)
    {
        RiftGameState->SetCurrentRoomPhase(RoomPhase);
        RiftGameState->SetAliveEnemyCount(AliveEnemyCount);
    }
}

void ARiftRoomManager::OnRep_RoomPhase()
{
}

void ARiftRoomManager::OnRep_AliveEnemyCount()
{
}
