#include "Core/RiftPlayerController.h"
#include "Core/RiftGameState.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
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

    if (IsLocalController())
    {
        SpawnRuntimeVisualFallbackLocal();
    }
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

void ARiftPlayerController::SpawnRuntimeVisualFallbackLocal()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (TActorIterator<AStaticMeshActor> It(World); It; ++It)
    {
        if (It->ActorHasTag(TEXT("RiftRuntimeVisualFallback")))
        {
            return;
        }
    }

    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh)
    {
        auto SpawnCube = [World, CubeMesh](const TCHAR* ActorName, const FVector& Location, const FVector& Scale)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Name = FName(ActorName);
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator, SpawnParams);
            if (!MeshActor)
            {
                return;
            }

            MeshActor->Tags.Add(TEXT("RiftRuntimeVisualFallback"));
            MeshActor->SetMobility(EComponentMobility::Movable);
            MeshActor->SetActorScale3D(Scale);

            UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent();
            if (MeshComponent)
            {
                MeshComponent->SetStaticMesh(CubeMesh);
                MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
            }
        };

        SpawnCube(TEXT("RS_RuntimeFloor"), FVector(0.0f, 0.0f, -12.0f), FVector(16.0f, 16.0f, 0.12f));
        SpawnCube(TEXT("RS_RuntimeWall_North"), FVector(0.0f, 820.0f, 90.0f), FVector(16.0f, 0.25f, 1.8f));
        SpawnCube(TEXT("RS_RuntimeWall_South"), FVector(0.0f, -820.0f, 90.0f), FVector(16.0f, 0.25f, 1.8f));
        SpawnCube(TEXT("RS_RuntimeWall_East"), FVector(820.0f, 0.0f, 90.0f), FVector(0.25f, 16.0f, 1.8f));
        SpawnCube(TEXT("RS_RuntimeWall_West"), FVector(-820.0f, 0.0f, 90.0f), FVector(0.25f, 16.0f, 1.8f));
    }

    FActorSpawnParameters LightSpawnParams;
    LightSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    APointLight* PointLight = World->SpawnActor<APointLight>(FVector(0.0f, 0.0f, 900.0f), FRotator::ZeroRotator, LightSpawnParams);
    UPointLightComponent* PointLightComponent = PointLight ? Cast<UPointLightComponent>(PointLight->GetLightComponent()) : nullptr;
    if (PointLightComponent)
    {
        PointLight->Tags.Add(TEXT("RiftRuntimeVisualFallback"));
        PointLightComponent->SetMobility(EComponentMobility::Movable);
        PointLightComponent->SetIntensity(45000.0f);
        PointLightComponent->SetAttenuationRadius(3600.0f);
        PointLightComponent->SetCastShadows(false);
    }

    ADirectionalLight* DirectionalLight = World->SpawnActor<ADirectionalLight>(
        FVector(0.0f, 0.0f, 1200.0f),
        FRotator(-55.0f, -35.0f, 0.0f),
        LightSpawnParams);
    UDirectionalLightComponent* DirectionalLightComponent = DirectionalLight ? DirectionalLight->GetComponent() : nullptr;
    if (DirectionalLightComponent)
    {
        DirectionalLight->Tags.Add(TEXT("RiftRuntimeVisualFallback"));
        DirectionalLightComponent->SetMobility(EComponentMobility::Movable);
        DirectionalLightComponent->SetIntensity(3.5f);
        DirectionalLightComponent->SetLightColor(FLinearColor(0.78f, 0.9f, 1.0f));
        DirectionalLightComponent->SetCastShadows(false);
    }

    ASkyLight* SkyLight = World->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator, LightSpawnParams);
    USkyLightComponent* SkyLightComponent = SkyLight ? SkyLight->GetLightComponent() : nullptr;
    if (SkyLightComponent)
    {
        SkyLight->Tags.Add(TEXT("RiftRuntimeVisualFallback"));
        SkyLightComponent->SetMobility(EComponentMobility::Movable);
        SkyLightComponent->SetIntensity(1.2f);
        SkyLightComponent->SetLightColor(FLinearColor(0.25f, 0.38f, 0.52f));
    }
}
