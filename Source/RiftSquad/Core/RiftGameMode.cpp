#include "Core/RiftGameMode.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Core/RiftGameState.h"
#include "Core/RiftPlayerController.h"

ARiftGameMode::ARiftGameMode()
{
    DefaultPawnClass = ARiftPlayerCharacter::StaticClass();
    PlayerControllerClass = ARiftPlayerController::StaticClass();
    GameStateClass = ARiftGameState::StaticClass();
}
