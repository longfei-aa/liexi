#include "Core/RiftGameState.h"
#include "Net/UnrealNetwork.h"

ARiftGameState::ARiftGameState()
{
    CurrentRunPhase = ERiftRunPhase::Setup;
    CurrentRoomPhase = ERiftRoomPhase::Idle;
    AliveEnemyCount = 0;
    CurrentRoomIndex = 0;
    TotalRoomCount = 0;
    LastRewardSummary = TEXT("No reward selected");
}

void ARiftGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARiftGameState, CurrentRunPhase);
    DOREPLIFETIME(ARiftGameState, CurrentRoomPhase);
    DOREPLIFETIME(ARiftGameState, AliveEnemyCount);
    DOREPLIFETIME(ARiftGameState, CurrentRoomIndex);
    DOREPLIFETIME(ARiftGameState, TotalRoomCount);
    DOREPLIFETIME(ARiftGameState, RewardOptions);
    DOREPLIFETIME(ARiftGameState, LastRewardSummary);
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

void ARiftGameState::SetCurrentRunPhase(ERiftRunPhase NewPhase)
{
    if (!HasAuthority())
    {
        return;
    }

    CurrentRunPhase = NewPhase;
    OnRep_CurrentRunPhase();
}

void ARiftGameState::SetRoomProgress(int32 NewCurrentRoomIndex, int32 NewTotalRoomCount)
{
    if (!HasAuthority())
    {
        return;
    }

    CurrentRoomIndex = FMath::Max(0, NewCurrentRoomIndex);
    TotalRoomCount = FMath::Max(0, NewTotalRoomCount);
    OnRep_RoomProgress();
}

void ARiftGameState::SetRewardOptions(const TArray<FRiftRewardOption>& NewRewardOptions)
{
    if (!HasAuthority())
    {
        return;
    }

    RewardOptions = NewRewardOptions;
    OnRep_RewardOptions();
}

void ARiftGameState::SetLastRewardSummary(const FString& NewSummary)
{
    if (!HasAuthority())
    {
        return;
    }

    LastRewardSummary = NewSummary;
    OnRep_LastRewardSummary();
}

void ARiftGameState::OnRep_CurrentRunPhase()
{
}

void ARiftGameState::OnRep_CurrentRoomPhase()
{
}

void ARiftGameState::OnRep_AliveEnemyCount()
{
}

void ARiftGameState::OnRep_RoomProgress()
{
}

void ARiftGameState::OnRep_RewardOptions()
{
}

void ARiftGameState::OnRep_LastRewardSummary()
{
}
