#include "UI/RiftCombatHUD.h"

#include "Combat/RiftHealthComponent.h"
#include "Core/RiftGameState.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

namespace
{
    FString RunPhaseToString(ERiftRunPhase Phase)
    {
        switch (Phase)
        {
            case ERiftRunPhase::Setup:
                return TEXT("Setup");
            case ERiftRunPhase::Combat:
                return TEXT("Combat");
            case ERiftRunPhase::Reward:
                return TEXT("Reward");
            case ERiftRunPhase::Victory:
                return TEXT("Victory");
            case ERiftRunPhase::Defeat:
                return TEXT("Defeat");
            default:
                return TEXT("Unknown");
        }
    }
}

void ARiftCombatHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas)
    {
        return;
    }

    const float Padding = 28.0f;
    float LineY = Padding;
    constexpr float LineHeight = 28.0f;

    DrawText(TEXT("Rift Squad Prototype"), FLinearColor::Yellow, Padding, LineY, GEngine ? GEngine->GetMediumFont() : nullptr, 1.0f);
    LineY += LineHeight * 1.4f;

    const ARiftGameState* RiftGameState = GetWorld() ? GetWorld()->GetGameState<ARiftGameState>() : nullptr;
    if (RiftGameState)
    {
        const FString RoomText = FString::Printf(
            TEXT("Room: %d / %d"),
            RiftGameState->GetCurrentRoomIndex(),
            RiftGameState->GetTotalRoomCount());
        DrawText(RoomText, FLinearColor::White, Padding, LineY, GEngine ? GEngine->GetSmallFont() : nullptr, 1.0f);
        LineY += LineHeight;

        const FString PhaseText = FString::Printf(TEXT("Phase: %s"), *RunPhaseToString(RiftGameState->GetCurrentRunPhase()));
        DrawText(PhaseText, FLinearColor::White, Padding, LineY, GEngine ? GEngine->GetSmallFont() : nullptr, 1.0f);
        LineY += LineHeight;

        const FString EnemyText = FString::Printf(TEXT("Enemies Left: %d"), RiftGameState->GetAliveEnemyCount());
        DrawText(EnemyText, FLinearColor::White, Padding, LineY, GEngine ? GEngine->GetSmallFont() : nullptr, 1.0f);
        LineY += LineHeight;
    }
    else
    {
        DrawText(TEXT("GameState not available"), FLinearColor::Red, Padding, LineY, GEngine ? GEngine->GetSmallFont() : nullptr, 1.0f);
        LineY += LineHeight;
    }

    const APawn* Pawn = GetOwningPawn();
    const URiftHealthComponent* HealthComponent = Pawn ? Pawn->FindComponentByClass<URiftHealthComponent>() : nullptr;
    if (HealthComponent)
    {
        const FString HealthText = FString::Printf(
            TEXT("HP: %.0f / %.0f"),
            HealthComponent->GetCurrentHealth(),
            HealthComponent->GetMaxHealth());
        DrawText(HealthText, FLinearColor::Green, Padding, LineY, GEngine ? GEngine->GetSmallFont() : nullptr, 1.0f);
    }
    else
    {
        DrawText(TEXT("Pawn/Health not available"), FLinearColor::Red, Padding, LineY, GEngine ? GEngine->GetSmallFont() : nullptr, 1.0f);
    }
}
