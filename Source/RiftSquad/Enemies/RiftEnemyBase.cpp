#include "Enemies/RiftEnemyBase.h"
#include "Combat/RiftHealthComponent.h"
#include "Combat/RiftProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Enemies/RiftEnemyAIController.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Rooms/RiftRoomManager.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ARiftEnemyBase::ARiftEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    AIControllerClass = ARiftEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    HealthComponent = CreateDefaultSubobject<URiftHealthComponent>(TEXT("HealthComponent"));
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -35.0f));
    VisualMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.8f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> EnemyMeshFinder(TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (EnemyMeshFinder.Succeeded())
    {
        VisualMesh->SetStaticMesh(EnemyMeshFinder.Object);
    }

    MoveSpeed = 360.0f;
    AttackRange = 175.0f;
    AttackDamage = 4.0f;
    AttackCooldown = 1.75f;
    ActivationDelay = 1.5f;
    PreferredRange = 520.0f;
    ExplosionRadius = 240.0f;
    ExplosionFuseSeconds = 0.85f;
    ProjectileClass = ARiftProjectile::StaticClass();
    EnemyType = ERiftEnemyType::Chaser;
    LastAttackTime = -1000.0f;
    SpawnTime = 0.0f;
    bExplosionTriggered = false;
    BaseVisualScale = FVector(0.5f, 0.5f, 0.8f);

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void ARiftEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        SpawnTime = World->GetTimeSeconds();
    }

    if (HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddDynamic(this, &ARiftEnemyBase::HandleHealthChanged);
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

    UWorld* World = GetWorld();
    if (!World || World->GetTimeSeconds() - SpawnTime < ActivationDelay)
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
    switch (EnemyType)
    {
        case ERiftEnemyType::Shooter:
            TickShooter(TargetPawn, Distance, ToTarget);
            break;
        case ERiftEnemyType::Burster:
            TickBurster(TargetPawn, Distance, ToTarget);
            break;
        case ERiftEnemyType::Elite:
        case ERiftEnemyType::Chaser:
        default:
            TickChaser(TargetPawn, Distance, ToTarget);
            break;
    }
}

void ARiftEnemyBase::SetOwningRoom(ARiftRoomManager* InRoomManager)
{
    OwningRoomManager = InRoomManager;
}

void ARiftEnemyBase::ConfigureEnemy(ERiftEnemyType NewEnemyType, float NewMaxHealth, float NewMoveSpeed, float NewAttackDamage, float NewAttackCooldown, FVector NewVisualScale)
{
    if (!HasAuthority())
    {
        return;
    }

    EnemyType = NewEnemyType;
    MoveSpeed = FMath::Max(50.0f, NewMoveSpeed);
    AttackDamage = FMath::Max(0.0f, NewAttackDamage);
    AttackCooldown = FMath::Max(0.1f, NewAttackCooldown);
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    if (HealthComponent)
    {
        HealthComponent->SetMaxHealth(NewMaxHealth, true);
    }

    if (VisualMesh)
    {
        VisualMesh->SetRelativeScale3D(NewVisualScale);
    }

    ApplyVisualForType();
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

void ARiftEnemyBase::TickChaser(APawn* TargetPawn, float Distance, FVector ToTarget)
{
    if (Distance > AttackRange)
    {
        AddMovementInput(ToTarget.GetSafeNormal2D());
    }
    else
    {
        TryAttack(TargetPawn);
    }
}

void ARiftEnemyBase::TickShooter(APawn* TargetPawn, float Distance, FVector ToTarget)
{
    const FVector DirectionToTarget = ToTarget.GetSafeNormal2D();
    if (Distance > PreferredRange)
    {
        AddMovementInput(DirectionToTarget);
    }
    else if (Distance < PreferredRange * 0.65f)
    {
        AddMovementInput(-DirectionToTarget);
    }

    FireProjectileAt(TargetPawn);
}

void ARiftEnemyBase::TickBurster(APawn* TargetPawn, float Distance, FVector ToTarget)
{
    if (bExplosionTriggered)
    {
        return;
    }

    if (Distance > AttackRange)
    {
        AddMovementInput(ToTarget.GetSafeNormal2D());
    }
    else
    {
        TriggerExplosion(TargetPawn);
    }
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

void ARiftEnemyBase::FireProjectileAt(APawn* TargetPawn)
{
    UWorld* World = GetWorld();
    if (!World || !TargetPawn || !ProjectileClass)
    {
        return;
    }

    const float Now = World->GetTimeSeconds();
    if (Now - LastAttackTime < AttackCooldown)
    {
        return;
    }

    LastAttackTime = Now;

    const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
    FVector FireDirection = ToTarget.GetSafeNormal();
    FireDirection.Z = 0.0f;
    FireDirection = FireDirection.GetSafeNormal();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    ARiftProjectile* Projectile = World->SpawnActor<ARiftProjectile>(
        ProjectileClass,
        GetActorLocation() + FireDirection * 70.0f + FVector(0.0f, 0.0f, 40.0f),
        FireDirection.Rotation(),
        SpawnParams);

    if (Projectile)
    {
        Projectile->InitializeProjectile(this, AttackDamage);
    }
}

void ARiftEnemyBase::TriggerExplosion(APawn* TargetPawn)
{
    if (bExplosionTriggered)
    {
        return;
    }

    bExplosionTriggered = true;
    GetCharacterMovement()->DisableMovement();

    if (VisualMesh)
    {
        VisualMesh->SetRelativeScale3D(VisualMesh->GetRelativeScale3D() * 1.45f);
    }

    GetWorldTimerManager().SetTimerForNextTick(this, &ARiftEnemyBase::ExecuteExplosion);
}

void ARiftEnemyBase::ExecuteExplosion()
{
    UWorld* World = GetWorld();
    if (!World || !HasAuthority())
    {
        return;
    }

    FCollisionShape ExplosionShape = FCollisionShape::MakeSphere(ExplosionRadius);
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RiftEnemyExplosion), false, this);

    TArray<FOverlapResult> Overlaps;
    World->OverlapMultiByObjectType(
        Overlaps,
        GetActorLocation(),
        FQuat::Identity,
        FCollisionObjectQueryParams(ECC_Pawn),
        ExplosionShape,
        QueryParams);

    TSet<AActor*> DamagedActors;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* Actor = Overlap.GetActor();
        if (!Actor || Actor == this || DamagedActors.Contains(Actor) || Cast<ARiftEnemyBase>(Actor))
        {
            continue;
        }

        if (URiftHealthComponent* TargetHealthComponent = Actor->FindComponentByClass<URiftHealthComponent>())
        {
            TargetHealthComponent->ApplyDamage(AttackDamage, this);
            DamagedActors.Add(Actor);
        }
    }

    if (HealthComponent && !HealthComponent->IsDead())
    {
        HealthComponent->ApplyDamage(HealthComponent->GetCurrentHealth(), this);
    }
}

void ARiftEnemyBase::ApplyVisualForType()
{
    if (!VisualMesh)
    {
        return;
    }

    switch (EnemyType)
    {
        case ERiftEnemyType::Shooter:
            VisualMesh->SetRelativeScale3D(FVector(0.45f, 0.45f, 0.55f));
            break;
        case ERiftEnemyType::Burster:
            VisualMesh->SetRelativeScale3D(FVector(0.65f, 0.65f, 0.65f));
            break;
        case ERiftEnemyType::Elite:
            VisualMesh->SetRelativeScale3D(FVector(0.82f, 0.82f, 1.2f));
            break;
        case ERiftEnemyType::Chaser:
        default:
            break;
    }

    BaseVisualScale = VisualMesh->GetRelativeScale3D();
}

void ARiftEnemyBase::ResetHitFeedback()
{
    if (!VisualMesh || bExplosionTriggered || (HealthComponent && HealthComponent->IsDead()))
    {
        return;
    }

    VisualMesh->SetRelativeScale3D(BaseVisualScale);
}

void ARiftEnemyBase::HandleHealthChanged(URiftHealthComponent* ChangedHealthComponent, float NewHealth, float Delta)
{
    if (HasAuthority() && Delta < -KINDA_SMALL_NUMBER && ChangedHealthComponent && !ChangedHealthComponent->IsDead())
    {
        MulticastPlayHitFeedback();
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
    MulticastPlayDeathFeedback();

    if (OwningRoomManager)
    {
        OwningRoomManager->NotifyEnemyKilled(this);
    }

    SetLifeSpan(3.0f);
}

void ARiftEnemyBase::MulticastPlayHitFeedback_Implementation()
{
    if (!VisualMesh || bExplosionTriggered || (HealthComponent && HealthComponent->IsDead()))
    {
        return;
    }

    GetWorldTimerManager().ClearTimer(HitFeedbackTimerHandle);
    VisualMesh->SetRelativeScale3D(BaseVisualScale * 1.18f);
    GetWorldTimerManager().SetTimer(HitFeedbackTimerHandle, this, &ARiftEnemyBase::ResetHitFeedback, 0.08f, false);
}

void ARiftEnemyBase::MulticastPlayDeathFeedback_Implementation()
{
    if (!VisualMesh)
    {
        return;
    }

    GetWorldTimerManager().ClearTimer(HitFeedbackTimerHandle);
    VisualMesh->SetRelativeScale3D(FVector(BaseVisualScale.X * 1.25f, BaseVisualScale.Y * 1.25f, BaseVisualScale.Z * 0.18f));
    VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -68.0f));
}
