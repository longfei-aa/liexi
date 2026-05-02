#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RiftCombatHUD.generated.h"

UCLASS()
class RIFTSQUAD_API ARiftCombatHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    void DrawPanel(float X, float Y, float W, float H, FLinearColor FillColor, FLinearColor BorderColor);
    void DrawProgressBar(float X, float Y, float W, float H, float Percent, FLinearColor FillColor, FLinearColor BackColor);
    void DrawCenteredText(const FString& Text, float Y, FLinearColor Color, UFont* Font, float Scale);
};
