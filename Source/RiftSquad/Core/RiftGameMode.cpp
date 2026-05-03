#include "Core/RiftGameMode.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Combat/RiftHealthComponent.h"
#include "Core/RiftGameState.h"
#include "Core/RiftPlayerController.h"
#include "EngineUtils.h"
#include "Rooms/RiftRoomManager.h"
#include "UI/RiftCombatHUD.h"
#include "UObject/ConstructorHelpers.h"

ARiftGameMode::ARiftGameMode()
{
    DefaultPawnClass = ARiftPlayerCharacter::StaticClass();
    PlayerControllerClass = ARiftPlayerController::StaticClass();
    GameStateClass = ARiftGameState::StaticClass();
    HUDClass = ARiftCombatHUD::StaticClass();

    bSpawnInitialRoomManager = true;
    RoomManagerClass = ARiftRoomManager::StaticClass();
}

void ARiftGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (!World || !bSpawnInitialRoomManager || !RoomManagerClass)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    World->SpawnActor<ARiftRoomManager>(RoomManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}

void ARiftGameMode::NotifyPlayerDied()
{
    if (!HasAuthority() || !AreAllPlayersDead())
    {
        return;
    }

    if (ARiftGameState* RiftGameState = GetGameState<ARiftGameState>())
    {
        RiftGameState->SetCurrentRunPhase(ERiftRunPhase::Defeat);
        RiftGameState->SetLastRewardSummary(TEXT("Squad wiped"));
    }

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<ARiftRoomManager> It(World); It; ++It)
        {
            It->MarkRunDefeated();
            return;
        }
    }
}

bool ARiftGameMode::AreAllPlayersDead() const
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    int32 PlayerCount = 0;
    int32 DeadCount = 0;
    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        const APlayerController* PlayerController = It->Get();
        const APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
        if (!Pawn)
        {
            return false;
        }

        const URiftHealthComponent* HealthComponent = Pawn ? Pawn->FindComponentByClass<URiftHealthComponent>() : nullptr;
        if (!HealthComponent)
        {
            return false;
        }

        PlayerCount++;
        if (HealthComponent->IsDead())
        {
            DeadCount++;
        }
    }

    return PlayerCount > 0 && PlayerCount == DeadCount;
}
