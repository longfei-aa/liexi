#include "Enemies/RiftEnemyBase.h"
#include "Combat/RiftHealthComponent.h"
#include "Enemies/RiftEnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Rooms/RiftRoomManager.h"

ARiftEnemyBase::ARiftEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    AIControllerClass = ARiftEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    HealthComponent = CreateDefaultSubobject<URiftHealthComponent>(TEXT("HealthComponent"));

    MoveSpeed = 360.0f;
    AttackRange = 175.0f;
    AttackDamage = 8.0f;
    AttackCooldown = 1.25f;
    LastAttackTime = -1000.0f;

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void ARiftEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &ARiftEnemyBase::HandleDeath);
    }
}

void ARiftEnemyBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!HasAuthority() || !HealthComponent || HealthComponent->IsDead())
    {
        return;
    }

    APawn* TargetPawn = FindTargetPawn();
    if (!TargetPawn)
    {
        return;
    }

    const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
    const float Distance = ToTarget.Size2D();
    if (Distance > AttackRange)
    {
        AddMovementInput(ToTarget.GetSafeNormal2D());
    }
    else
    {
        TryAttack(TargetPawn);
    }
}

void ARiftEnemyBase::SetOwningRoom(ARiftRoomManager* InRoomManager)
{
    OwningRoomManager = InRoomManager;
}

APawn* ARiftEnemyBase::FindTargetPawn() const
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    APawn* BestPawn = nullptr;
    float BestDistanceSq = TNumericLimits<float>::Max();

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        const APlayerController* PlayerController = It->Get();
        APawn* Candidate = PlayerController ? PlayerController->GetPawn() : nullptr;
        if (!Candidate)
        {
            continue;
        }

        const float DistanceSq = FVector::DistSquared2D(GetActorLocation(), Candidate->GetActorLocation());
        if (DistanceSq < BestDistanceSq)
        {
            BestDistanceSq = DistanceSq;
            BestPawn = Candidate;
        }
    }

    return BestPawn;
}

void ARiftEnemyBase::TryAttack(APawn* TargetPawn)
{
    UWorld* World = GetWorld();
    if (!World || !TargetPawn)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();
    if (Now - LastAttackTime < AttackCooldown)
    {
        return;
    }

    LastAttackTime = Now;

    if (URiftHealthComponent* TargetHealth = TargetPawn->FindComponentByClass<URiftHealthComponent>())
    {
        TargetHealth->ApplyDamage(AttackDamage, this);
    }
}

void ARiftEnemyBase::HandleDeath(URiftHealthComponent* DeadHealthComponent, AActor* DamageInstigator)
{
    if (!HasAuthority())
    {
        return;
    }

    SetActorEnableCollision(false);
    GetCharacterMovement()->DisableMovement();

    if (OwningRoomManager)
    {
        OwningRoomManager->NotifyEnemyKilled(this);
    }

    SetLifeSpan(3.0f);
}
