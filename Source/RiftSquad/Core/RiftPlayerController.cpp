#include "Core/RiftPlayerController.h"
#include "Core/RiftGameState.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Rooms/RiftRoomManager.h"

namespace
{
    constexpr int32 MainMenuOptionCount = 5;
    constexpr int32 PauseMenuOptionCount = 4;
}

ARiftPlayerController::ARiftPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
    MainMenuSelectionIndex = 0;
    MenuStatusMessage = TEXT("Select an option with Up/Down, confirm with Enter.");
    bPauseMenuOpen = false;
    PauseMenuSelectionIndex = 0;
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

    FInputActionBinding& ConfirmBinding = InputComponent->BindAction(TEXT("StartRun"), IE_Pressed, this, &ARiftPlayerController::ConfirmMenuSelection);
    ConfirmBinding.bExecuteWhenPaused = true;

    FInputActionBinding& MenuUpBinding = InputComponent->BindAction(TEXT("MenuUp"), IE_Pressed, this, &ARiftPlayerController::SelectPreviousMenuOption);
    MenuUpBinding.bExecuteWhenPaused = true;

    FInputActionBinding& MenuDownBinding = InputComponent->BindAction(TEXT("MenuDown"), IE_Pressed, this, &ARiftPlayerController::SelectNextMenuOption);
    MenuDownBinding.bExecuteWhenPaused = true;

    FInputActionBinding& PauseBinding = InputComponent->BindAction(TEXT("PauseMenu"), IE_Pressed, this, &ARiftPlayerController::TogglePauseMenu);
    PauseBinding.bExecuteWhenPaused = true;

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

void ARiftPlayerController::ServerReturnToTitle_Implementation()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<ARiftRoomManager> It(World); It; ++It)
    {
        It->ReturnToTitle();
        return;
    }
}

void ARiftPlayerController::ConfirmMenuSelection()
{
    if (bPauseMenuOpen)
    {
        switch (PauseMenuSelectionIndex)
        {
            case 0:
                TogglePauseMenu();
                break;
            case 1:
                UGameplayStatics::SetGamePaused(this, false);
                bPauseMenuOpen = false;
                PauseMenuSelectionIndex = 0;
                MenuStatusMessage = TEXT("Restarting run...");
                ServerStartRun();
                break;
            case 2:
                UGameplayStatics::SetGamePaused(this, false);
                bPauseMenuOpen = false;
                PauseMenuSelectionIndex = 0;
                MainMenuSelectionIndex = 0;
                MenuStatusMessage = TEXT("Returned to title.");
                ServerReturnToTitle();
                break;
            case 3:
                ConsoleCommand(TEXT("quit"));
                break;
            default:
                break;
        }
        return;
    }

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
    if (bPauseMenuOpen)
    {
        PauseMenuSelectionIndex = (PauseMenuSelectionIndex + PauseMenuOptionCount - 1) % PauseMenuOptionCount;
        return;
    }

    if (GetCurrentRunPhase() != ERiftRunPhase::Setup)
    {
        return;
    }

    MainMenuSelectionIndex = (MainMenuSelectionIndex + MainMenuOptionCount - 1) % MainMenuOptionCount;
    MenuStatusMessage = TEXT("Select an option with Up/Down, confirm with Enter.");
}

void ARiftPlayerController::SelectNextMenuOption()
{
    if (bPauseMenuOpen)
    {
        PauseMenuSelectionIndex = (PauseMenuSelectionIndex + 1) % PauseMenuOptionCount;
        return;
    }

    if (GetCurrentRunPhase() != ERiftRunPhase::Setup)
    {
        return;
    }

    MainMenuSelectionIndex = (MainMenuSelectionIndex + 1) % MainMenuOptionCount;
    MenuStatusMessage = TEXT("Select an option with Up/Down, confirm with Enter.");
}

void ARiftPlayerController::TogglePauseMenu()
{
    const ERiftRunPhase RunPhase = GetCurrentRunPhase();
    const bool bCanTogglePause = RunPhase == ERiftRunPhase::Combat || RunPhase == ERiftRunPhase::Reward || RunPhase == ERiftRunPhase::Supply;
    if (!bCanTogglePause && !bPauseMenuOpen)
    {
        return;
    }

    bPauseMenuOpen = !bPauseMenuOpen;
    if (bPauseMenuOpen)
    {
        PauseMenuSelectionIndex = 0;
    }

    UGameplayStatics::SetGamePaused(this, bPauseMenuOpen);
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
