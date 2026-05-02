#include "Core/RiftGameMode.h"
#include "Characters/RiftPlayerCharacter.h"
#include "Combat/RiftHealthComponent.h"
#include "Core/RiftGameState.h"
#include "Core/RiftPlayerController.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/PointLight.h"
#include "Engine/StaticMeshActor.h"
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
    bSpawnRuntimeVisualFallback = true;
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

    if (bSpawnRuntimeVisualFallback)
    {
        SpawnRuntimeVisualFallback();
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    World->SpawnActor<ARiftRoomManager>(RoomManagerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}

void ARiftGameMode::SpawnRuntimeVisualFallback()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
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
        PointLightComponent->SetIntensity(80000.0f);
        PointLightComponent->SetAttenuationRadius(3000.0f);
    }
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
        const URiftHealthComponent* HealthComponent = Pawn ? Pawn->FindComponentByClass<URiftHealthComponent>() : nullptr;
        if (!HealthComponent)
        {
            continue;
        }

        PlayerCount++;
        if (HealthComponent->IsDead())
        {
            DeadCount++;
        }
    }

    return PlayerCount > 0 && PlayerCount == DeadCount;
}
