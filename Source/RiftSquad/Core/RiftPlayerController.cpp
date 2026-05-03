#include "Core/RiftPlayerController.h"
#include "Core/RiftGameState.h"
#include "EngineUtils.h"
#include "Rooms/RiftRoomManager.h"

namespace
{
    constexpr int32 MainMenuOptionCount = 5;
}

ARiftPlayerController::ARiftPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
    MainMenuSelectionIndex = 0;
    MenuStatusMessage = TEXT("Select an option with Up/Down, confirm with Enter.");
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

    InputComponent->BindAction(TEXT("StartRun"), IE_Pressed, this, &ARiftPlayerController::ConfirmMenuSelection);
    InputComponent->BindAction(TEXT("MenuUp"), IE_Pressed, this, &ARiftPlayerController::SelectPreviousMenuOption);
    InputComponent->BindAction(TEXT("MenuDown"), IE_Pressed, this, &ARiftPlayerController::SelectNextMenuOption);
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

void ARiftPlayerController::ServerStartRun_Implementation()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<ARiftRoomManager> It(World); It; ++It)
    {
        It->StartRun();
        return;
    }
}

void ARiftPlayerController::ConfirmMenuSelection()
{
    const ERiftRunPhase RunPhase = GetCurrentRunPhase();
    if (RunPhase == ERiftRunPhase::Victory || RunPhase == ERiftRunPhase::Defeat)
    {
        MenuStatusMessage = TEXT("Restarting run...");
        ServerStartRun();
        return;
    }

    if (RunPhase != ERiftRunPhase::Setup)
    {
        return;
    }

    switch (MainMenuSelectionIndex)
    {
        case 0:
            MenuStatusMessage = TEXT("Deploying current combat demo...");
            ServerStartRun();
            break;
        case 1:
            MenuStatusMessage = TEXT("Host Game is the next networking milestone. Use Start Demo for now.");
            break;
        case 2:
            MenuStatusMessage = TEXT("Join Game placeholder. Friend-room flow will be implemented after PIE network validation.");
            break;
        case 3:
            MenuStatusMessage = TEXT("Settings placeholder. Audio/window/input options will move here.");
            break;
        case 4:
            ConsoleCommand(TEXT("quit"));
            break;
        default:
            break;
    }
}

void ARiftPlayerController::SelectPreviousMenuOption()
{
    if (GetCurrentRunPhase() != ERiftRunPhase::Setup)
    {
        return;
    }

    MainMenuSelectionIndex = (MainMenuSelectionIndex + MainMenuOptionCount - 1) % MainMenuOptionCount;
    MenuStatusMessage = TEXT("Select an option with Up/Down, confirm with Enter.");
}

void ARiftPlayerController::SelectNextMenuOption()
{
    if (GetCurrentRunPhase() != ERiftRunPhase::Setup)
    {
        return;
    }

    MainMenuSelectionIndex = (MainMenuSelectionIndex + 1) % MainMenuOptionCount;
    MenuStatusMessage = TEXT("Select an option with Up/Down, confirm with Enter.");
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

ERiftRunPhase ARiftPlayerController::GetCurrentRunPhase() const
{
    const ARiftGameState* RiftGameState = GetWorld() ? GetWorld()->GetGameState<ARiftGameState>() : nullptr;
    return RiftGameState ? RiftGameState->GetCurrentRunPhase() : ERiftRunPhase::Setup;
}
