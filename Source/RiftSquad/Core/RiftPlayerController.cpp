#include "Core/RiftPlayerController.h"
#include "EngineUtils.h"
#include "Rooms/RiftRoomManager.h"

ARiftPlayerController::ARiftPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ARiftPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void ARiftPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!InputComponent)
    {
        return;
    }

    InputComponent->BindAction(TEXT("RewardOne"), IE_Pressed, this, &ARiftPlayerController::SelectRewardOne);
    InputComponent->BindAction(TEXT("RewardTwo"), IE_Pressed, this, &ARiftPlayerController::SelectRewardTwo);
    InputComponent->BindAction(TEXT("RewardThree"), IE_Pressed, this, &ARiftPlayerController::SelectRewardThree);
}

void ARiftPlayerController::ServerRequestInteract_Implementation(AActor* TargetActor)
{
}

void ARiftPlayerController::ServerSelectReward_Implementation(int32 OptionIndex)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<ARiftRoomManager> It(World); It; ++It)
    {
        It->SelectRewardForPlayer(this, OptionIndex);
        return;
    }
}

void ARiftPlayerController::SelectRewardOne()
{
    ServerSelectReward(0);
}

void ARiftPlayerController::SelectRewardTwo()
{
    ServerSelectReward(1);
}

void ARiftPlayerController::SelectRewardThree()
{
    ServerSelectReward(2);
}
