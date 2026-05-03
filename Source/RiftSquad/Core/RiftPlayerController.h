#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RiftPlayerController.generated.h"

UCLASS()
class RIFTSQUAD_API ARiftPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARiftPlayerController();

    virtual void BeginPlay() override;

    UFUNCTION(Server, Reliable)
    void ServerRequestInteract(AActor* TargetActor);

    UFUNCTION(Server, Reliable)
    void ServerSelectReward(int32 OptionIndex);

    UFUNCTION(Server, Reliable)
    void ServerStartRun();

protected:
    virtual void SetupInputComponent() override;

    void StartRun();
    void SelectRewardOne();
    void SelectRewardTwo();
    void SelectRewardThree();
};
