#include "Core/RiftPlayerController.h"

ARiftPlayerController::ARiftPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ARiftPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ARiftPlayerController::ServerRequestInteract_Implementation(AActor* TargetActor)
{
}
