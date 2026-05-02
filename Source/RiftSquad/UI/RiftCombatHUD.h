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
};
