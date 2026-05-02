#include "Core/RiftGameMode.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Core/RiftGameState.h"
#include "Core/RiftPlayerController.h"
#include "Rooms/RiftRoomManager.h"

ARiftGameMode::ARiftGameMode()
{
    DefaultPawnClass = ARiftPlayerCharacter::StaticClass();
    PlayerControllerClass = ARiftPlayerController::StaticClass();
    GameStateClass = ARiftGameState::StaticClass();

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
