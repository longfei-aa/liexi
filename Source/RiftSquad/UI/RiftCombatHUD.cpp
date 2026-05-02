#include "UI/RiftCombatHUD.h"

#include "Combat/RiftHealthComponent.h"
#include "Combat/RiftWeaponComponent.h"
#include "Core/RiftGameState.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "GameFramework/Pawn.h"

namespace
{
    FLinearColor PhaseColor(ERiftRunPhase Phase)
    {
        switch (Phase)
        {
            case ERiftRunPhase::Combat:
                return FLinearColor(1.0f, 0.28f, 0.18f, 1.0f);
            case ERiftRunPhase::Reward:
                return FLinearColor(1.0f, 0.78f, 0.22f, 1.0f);
            case ERiftRunPhase::Victory:
                return FLinearColor(0.18f, 1.0f, 0.45f, 1.0f);
            case ERiftRunPhase::Defeat:
                return FLinearColor(1.0f, 0.08f, 0.08f, 1.0f);
            default:
                return FLinearColor(0.45f, 0.8f, 1.0f, 1.0f);
        }
    }

    FString RunPhaseToString(ERiftRunPhase Phase)
    {
        switch (Phase)
        {
            case ERiftRunPhase::Setup:
                return TEXT("SETUP");
            case ERiftRunPhase::Combat:
                return TEXT("COMBAT");
            case ERiftRunPhase::Reward:
                return TEXT("REWARD");
            case ERiftRunPhase::Victory:
                return TEXT("VICTORY");
            case ERiftRunPhase::Defeat:
                return TEXT("DEFEAT");
            default:
                return TEXT("UNKNOWN");
        }
    }

    FString ObjectiveText(ERiftRunPhase Phase)
    {
        switch (Phase)
        {
            case ERiftRunPhase::Combat:
                return TEXT("Objective: eliminate all enemies in the room.");
            case ERiftRunPhase::Reward:
                return TEXT("Objective: choose one upgrade to continue.");
            case ERiftRunPhase::Victory:
                return TEXT("Objective complete: demo run cleared.");
            case ERiftRunPhase::Defeat:
                return TEXT("Objective failed: squad wiped.");
            default:
                return TEXT("Objective: prepare for deployment.");
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

    UFont* SmallFont = GEngine ? GEngine->GetSmallFont() : nullptr;
    UFont* MediumFont = GEngine ? GEngine->GetMediumFont() : nullptr;
    const float CanvasW = Canvas->SizeX;
    const float CanvasH = Canvas->SizeY;
    const float Safe = 28.0f;

    const ARiftGameState* RiftGameState = GetWorld() ? GetWorld()->GetGameState<ARiftGameState>() : nullptr;
    const APawn* Pawn = GetOwningPawn();
    const URiftHealthComponent* HealthComponent = Pawn ? Pawn->FindComponentByClass<URiftHealthComponent>() : nullptr;
    const URiftWeaponComponent* WeaponComponent = Pawn ? Pawn->FindComponentByClass<URiftWeaponComponent>() : nullptr;

    const ERiftRunPhase RunPhase = RiftGameState ? RiftGameState->GetCurrentRunPhase() : ERiftRunPhase::Setup;
    const FLinearColor Accent = PhaseColor(RunPhase);

    DrawPanel(Safe, Safe, 430.0f, 210.0f, FLinearColor(0.015f, 0.023f, 0.035f, 0.82f), Accent);
    DrawText(TEXT("RIFT SQUAD"), Accent, Safe + 22.0f, Safe + 18.0f, MediumFont, 1.25f);
    DrawText(TEXT("Playable Combat Demo"), FLinearColor(0.72f, 0.84f, 0.92f, 1.0f), Safe + 24.0f, Safe + 56.0f, SmallFont, 1.0f);

    const FString PhaseText = FString::Printf(TEXT("STATUS  %s"), *RunPhaseToString(RunPhase));
    DrawText(PhaseText, FLinearColor::White, Safe + 24.0f, Safe + 92.0f, SmallFont, 1.0f);

    if (RiftGameState)
    {
        const int32 CurrentRoom = RiftGameState->GetCurrentRoomIndex();
        const int32 TotalRooms = FMath::Max(1, RiftGameState->GetTotalRoomCount());
        const FString RoomText = FString::Printf(TEXT("ROOM    %d / %d"), CurrentRoom, TotalRooms);
        DrawText(RoomText, FLinearColor::White, Safe + 24.0f, Safe + 120.0f, SmallFont, 1.0f);

        const float RoomPercent = FMath::Clamp(static_cast<float>(CurrentRoom) / static_cast<float>(TotalRooms), 0.0f, 1.0f);
        DrawProgressBar(Safe + 24.0f, Safe + 150.0f, 372.0f, 12.0f, RoomPercent, Accent, FLinearColor(0.09f, 0.11f, 0.14f, 0.95f));

        const FString EnemyText = FString::Printf(TEXT("ENEMIES LEFT  %d"), RiftGameState->GetAliveEnemyCount());
        DrawText(EnemyText, FLinearColor(0.88f, 0.94f, 1.0f, 1.0f), Safe + 24.0f, Safe + 170.0f, SmallFont, 1.0f);
    }
    else
    {
        DrawText(TEXT("GameState unavailable"), FLinearColor::Red, Safe + 24.0f, Safe + 120.0f, SmallFont, 1.0f);
    }

    DrawPanel(Safe, CanvasH - 156.0f, 520.0f, 118.0f, FLinearColor(0.015f, 0.023f, 0.035f, 0.82f), FLinearColor(0.22f, 0.36f, 0.48f, 1.0f));
    DrawText(TEXT("OPERATIVE"), FLinearColor(0.72f, 0.84f, 0.92f, 1.0f), Safe + 22.0f, CanvasH - 136.0f, SmallFont, 1.0f);

    if (HealthComponent)
    {
        const float MaxHealth = FMath::Max(1.0f, HealthComponent->GetMaxHealth());
        const float HealthPercent = FMath::Clamp(HealthComponent->GetCurrentHealth() / MaxHealth, 0.0f, 1.0f);
        const FString HealthText = FString::Printf(TEXT("HP %.0f / %.0f"), HealthComponent->GetCurrentHealth(), MaxHealth);
        DrawText(HealthText, FLinearColor::White, Safe + 22.0f, CanvasH - 108.0f, SmallFont, 1.0f);
        DrawProgressBar(Safe + 132.0f, CanvasH - 102.0f, 350.0f, 15.0f, HealthPercent, FLinearColor(0.18f, 0.95f, 0.42f, 1.0f), FLinearColor(0.12f, 0.1f, 0.1f, 0.95f));
    }
    else
    {
        DrawText(TEXT("HP unavailable"), FLinearColor::Red, Safe + 22.0f, CanvasH - 108.0f, SmallFont, 1.0f);
    }

    if (WeaponComponent)
    {
        const FString WeaponText = FString::Printf(TEXT("DMG %.0f     FIRE %.2fs"), WeaponComponent->GetDamage(), WeaponComponent->GetFireInterval());
        DrawText(WeaponText, FLinearColor(0.78f, 0.92f, 1.0f, 1.0f), Safe + 22.0f, CanvasH - 75.0f, SmallFont, 1.0f);
    }

    const FString ControlsText = TEXT("WASD move   Mouse aim   LMB fire   1/2/3 choose reward");
    DrawText(ControlsText, FLinearColor(0.65f, 0.74f, 0.8f, 1.0f), Safe + 22.0f, CanvasH - 48.0f, SmallFont, 1.0f);

    const FString Objective = ObjectiveText(RunPhase);
    const float ObjectiveW = 560.0f;
    DrawPanel((CanvasW - ObjectiveW) * 0.5f, Safe, ObjectiveW, 58.0f, FLinearColor(0.015f, 0.023f, 0.035f, 0.72f), Accent);
    DrawCenteredText(Objective, Safe + 18.0f, FLinearColor::White, SmallFont, 1.0f);

    if (RiftGameState && !RiftGameState->GetLastRewardSummary().IsEmpty())
    {
        DrawText(RiftGameState->GetLastRewardSummary(), FLinearColor(0.55f, 0.85f, 1.0f, 1.0f), Safe, Safe + 232.0f, SmallFont, 1.0f);
    }

    if (RiftGameState && RunPhase == ERiftRunPhase::Reward)
    {
        const TArray<FRiftRewardOption>& RewardOptions = RiftGameState->GetRewardOptions();
        const float CardW = 300.0f;
        const float CardH = 150.0f;
        const float Gap = 18.0f;
        const float StartX = CanvasW - Safe - CardW;
        float CardY = Safe + 96.0f;

        DrawText(TEXT("CHOOSE UPGRADE"), Accent, StartX, CardY - 34.0f, MediumFont, 1.0f);
        for (int32 OptionIndex = 0; OptionIndex < RewardOptions.Num(); ++OptionIndex)
        {
            const FRiftRewardOption& Option = RewardOptions[OptionIndex];
            DrawPanel(StartX, CardY, CardW, CardH, FLinearColor(0.02f, 0.03f, 0.045f, 0.9f), OptionIndex == 0 ? Accent : FLinearColor(0.24f, 0.32f, 0.4f, 1.0f));

            const FString KeyText = FString::Printf(TEXT("[%d]"), OptionIndex + 1);
            DrawText(KeyText, Accent, StartX + 18.0f, CardY + 18.0f, MediumFont, 1.05f);
            DrawText(Option.Name, FLinearColor::White, StartX + 76.0f, CardY + 22.0f, SmallFont, 1.0f);
            DrawText(Option.Description, FLinearColor(0.74f, 0.84f, 0.9f, 1.0f), StartX + 76.0f, CardY + 56.0f, SmallFont, 1.0f);

            const FString MagnitudeText = FString::Printf(TEXT("Value %.2f"), Option.Magnitude);
            DrawText(MagnitudeText, FLinearColor(0.56f, 0.86f, 1.0f, 1.0f), StartX + 76.0f, CardY + 92.0f, SmallFont, 1.0f);
            CardY += CardH + Gap;
        }
    }

    if (RunPhase == ERiftRunPhase::Victory)
    {
        DrawPanel(CanvasW * 0.5f - 310.0f, CanvasH * 0.5f - 70.0f, 620.0f, 140.0f, FLinearColor(0.02f, 0.06f, 0.035f, 0.9f), Accent);
        DrawCenteredText(TEXT("VICTORY"), CanvasH * 0.5f - 42.0f, Accent, MediumFont, 2.0f);
        DrawCenteredText(TEXT("Prototype run complete. Core loop is playable."), CanvasH * 0.5f + 18.0f, FLinearColor::White, SmallFont, 1.0f);
    }
    else if (RunPhase == ERiftRunPhase::Defeat)
    {
        DrawPanel(CanvasW * 0.5f - 310.0f, CanvasH * 0.5f - 70.0f, 620.0f, 140.0f, FLinearColor(0.08f, 0.015f, 0.02f, 0.9f), Accent);
        DrawCenteredText(TEXT("DEFEAT"), CanvasH * 0.5f - 42.0f, Accent, MediumFont, 2.0f);
        DrawCenteredText(TEXT("Squad wiped. Restart Play to try the current demo again."), CanvasH * 0.5f + 18.0f, FLinearColor::White, SmallFont, 1.0f);
    }
}

void ARiftCombatHUD::DrawPanel(float X, float Y, float W, float H, FLinearColor FillColor, FLinearColor BorderColor)
{
    DrawRect(FillColor, X, Y, W, H);
    DrawRect(BorderColor, X, Y, W, 2.0f);
    DrawRect(BorderColor, X, Y + H - 2.0f, W, 2.0f);
    DrawRect(BorderColor, X, Y, 2.0f, H);
    DrawRect(BorderColor, X + W - 2.0f, Y, 2.0f, H);
}

void ARiftCombatHUD::DrawProgressBar(float X, float Y, float W, float H, float Percent, FLinearColor FillColor, FLinearColor BackColor)
{
    DrawRect(BackColor, X, Y, W, H);
    DrawRect(FillColor, X, Y, W * FMath::Clamp(Percent, 0.0f, 1.0f), H);
}

void ARiftCombatHUD::DrawCenteredText(const FString& Text, float Y, FLinearColor Color, UFont* Font, float Scale)
{
    if (!Canvas)
    {
        return;
    }

    float TextW = 0.0f;
    float TextH = 0.0f;
    GetTextSize(Text, TextW, TextH, Font, Scale);
    DrawText(Text, Color, (Canvas->SizeX - TextW) * 0.5f, Y, Font, Scale);
}
