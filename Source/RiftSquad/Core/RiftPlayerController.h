#pragma once

#include "CoreMinimal.h"
#include "Core/RiftTypes.h"
#include "GameFramework/PlayerController.h"
#include "RiftPlayerController.generated.h"

UCLASS()
class RIFTSQUAD_API ARiftPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARiftPlayerController();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure, Category = "Rift|Menu")
    int32 GetMainMenuSelectionIndex() const { return MainMenuSelectionIndex; }

    UFUNCTION(BlueprintPure, Category = "Rift|Menu")
    FString GetMenuStatusMessage() const { return MenuStatusMessage; }

    UFUNCTION(BlueprintPure, Category = "Rift|Menu")
    bool IsPauseMenuOpen() const { return bPauseMenuOpen; }

    UFUNCTION(BlueprintPure, Category = "Rift|Menu")
    int32 GetPauseMenuSelectionIndex() const { return PauseMenuSelectionIndex; }

    UFUNCTION(Server, Reliable)
    void ServerRequestInteract(AActor* TargetActor);

    UFUNCTION(Server, Reliable)
    void ServerSelectReward(int32 OptionIndex);

    UFUNCTION(Server, Reliable)
    void ServerStartRun();

    UFUNCTION(Server, Reliable)
    void ServerReturnToTitle();

protected:
    virtual void SetupInputComponent() override;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Menu")
    int32 MainMenuSelectionIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Menu")
    FString MenuStatusMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Menu")
    bool bPauseMenuOpen;

    UPROPERTY(BlueprintReadOnly, Category = "Rift|Menu")
    int32 PauseMenuSelectionIndex;

    void ConfirmMenuSelection();
    void SelectPreviousMenuOption();
    void SelectNextMenuOption();
    void TogglePauseMenu();
    void SelectRewardOne();
    void SelectRewardTwo();
    void SelectRewardThree();

    ERiftRunPhase GetCurrentRunPhase() const;
    void SpawnRuntimeVisualFallbackLocal();
};
