#include "Core/RiftGameState.h"
#include "Net/UnrealNetwork.h"

ARiftGameState::ARiftGameState()
{
    CurrentRoomPhase = ERiftRoomPhase::Idle;
    AliveEnemyCount = 0;
}

void ARiftGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARiftGameState, CurrentRoomPhase);
    DOREPLIFETIME(ARiftGameState, AliveEnemyCount);
}

void ARiftGameState::SetCurrentRoomPhase(ERiftRoomPhase NewPhase)
{
    if (!HasAuthority())
    {
        return;
    }

    CurrentRoomPhase = NewPhase;
    OnRep_CurrentRoomPhase();
}

void ARiftGameState::SetAliveEnemyCount(int32 NewCount)
{
    if (!HasAuthority())
    {
        return;
    }

    AliveEnemyCount = FMath::Max(0, NewCount);
    OnRep_AliveEnemyCount();
}

void ARiftGameState::OnRep_CurrentRoomPhase()
{
}

void ARiftGameState::OnRep_AliveEnemyCount()
{
}
