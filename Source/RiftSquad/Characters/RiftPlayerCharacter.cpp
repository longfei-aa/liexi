#include "Characters/RiftPlayerCharacter.h"
#include "Abilities/RiftAbilityComponent.h"
#include "Camera/CameraComponent.h"
#include "Combat/RiftHealthComponent.h"
#include "Combat/RiftWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/RiftGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

ARiftPlayerCharacter::ARiftPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 900.0f;
    CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;

    TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
    TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

    HealthComponent = CreateDefaultSubobject<URiftHealthComponent>(TEXT("HealthComponent"));
    WeaponComponent = CreateDefaultSubobject<URiftWeaponComponent>(TEXT("WeaponComponent"));
    AbilityComponent = CreateDefaultSubobject<URiftAbilityComponent>(TEXT("AbilityComponent"));

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
    VisualMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 1.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlayerMeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (PlayerMeshFinder.Succeeded())
    {
        VisualMesh->SetStaticMesh(PlayerMeshFinder.Object);
    }

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultContextFinder(TEXT("/Game/RiftSquad/Input/IMC_Player.IMC_Player"));
    if (DefaultContextFinder.Succeeded())
    {
        DefaultMappingContext = DefaultContextFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(TEXT("/Game/RiftSquad/Input/IA_Move.IA_Move"));
    if (MoveActionFinder.Succeeded())
    {
        MoveAction = MoveActionFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> FireActionFinder(TEXT("/Game/RiftSquad/Input/IA_Fire.IA_Fire"));
    if (FireActionFinder.Succeeded())
    {
        FireAction = FireActionFinder.Object;
    }
}

void ARiftPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &ARiftPlayerCharacter::HandleDeath);
        if (HasAuthority())
        {
            HealthComponent->SetMaxHealth(220.0f, true);
        }
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, 0);
                }
            }
        }
    }
}

void ARiftPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ARiftPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ARiftPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ARiftPlayerCharacter::FireLegacy);
    PlayerInputComponent->BindAction(TEXT("AbilityOne"), IE_Pressed, this, &ARiftPlayerCharacter::UseAbilityLegacy);
    PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ARiftPlayerCharacter::DashLegacy);

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInput)
    {
        return;
    }

    if (MoveAction)
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARiftPlayerCharacter::Move);
    }

    if (FireAction)
    {
        EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &ARiftPlayerCharacter::Fire);
    }
}

void ARiftPlayerCharacter::MultiplyMoveSpeed(float Multiplier)
{
    if (!HasAuthority())
    {
        return;
    }

    GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(GetCharacterMovement()->MaxWalkSpeed * Multiplier, 250.0f, 1200.0f);
}

float ARiftPlayerCharacter::GetCurrentMoveSpeed() const
{
    return GetCharacterMovement() ? GetCharacterMovement()->MaxWalkSpeed : 0.0f;
}

void ARiftPlayerCharacter::Move(const FInputActionValue& Value)
{
    if (HealthComponent && HealthComponent->IsDead())
    {
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();
    AddMovementInput(FVector::ForwardVector, MovementVector.Y);
    AddMovementInput(FVector::RightVector, MovementVector.X);
}

void ARiftPlayerCharacter::Fire(const FInputActionValue& Value)
{
    if (HealthComponent && HealthComponent->IsDead())
    {
        return;
    }

    const FVector AimDirection = GetAimDirection();
    SetActorRotation(AimDirection.Rotation());

    if (WeaponComponent)
    {
        WeaponComponent->RequestFire(GetActorLocation(), AimDirection);
    }
}

void ARiftPlayerCharacter::MoveForward(float Value)
{
    if (HealthComponent && HealthComponent->IsDead())
    {
        return;
    }

    AddMovementInput(FVector::ForwardVector, Value);
}

void ARiftPlayerCharacter::MoveRight(float Value)
{
    if (HealthComponent && HealthComponent->IsDead())
    {
        return;
    }

    AddMovementInput(FVector::RightVector, Value);
}

void ARiftPlayerCharacter::FireLegacy()
{
    if (HealthComponent && HealthComponent->IsDead())
    {
        return;
    }

    const FVector AimDirection = GetAimDirection();
    SetActorRotation(AimDirection.Rotation());

    if (WeaponComponent)
    {
        WeaponComponent->RequestFire(GetActorLocation(), AimDirection);
    }
}

void ARiftPlayerCharacter::UseAbilityLegacy()
{
    if ((HealthComponent && HealthComponent->IsDead()) || !AbilityComponent)
    {
        return;
    }

    AbilityComponent->RequestShockwave(GetActorLocation());
}

void ARiftPlayerCharacter::DashLegacy()
{
    if ((HealthComponent && HealthComponent->IsDead()) || !AbilityComponent)
    {
        return;
    }

    FVector DashDirection = GetLastMovementInputVector().GetSafeNormal2D();
    if (DashDirection.IsNearlyZero())
    {
        DashDirection = GetAimDirection();
    }

    AbilityComponent->RequestDash(DashDirection);
}

FVector ARiftPlayerCharacter::GetAimDirection() const
{
    const APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController)
    {
        return GetActorForwardVector();
    }

    FVector WorldLocation;
    FVector WorldDirection;
    if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
    {
        return GetActorForwardVector();
    }

    const float PlaneZ = GetActorLocation().Z;
    if (FMath::IsNearlyZero(WorldDirection.Z))
    {
        return GetActorForwardVector();
    }

    const float Distance = (PlaneZ - WorldLocation.Z) / WorldDirection.Z;
    const FVector AimPoint = WorldLocation + WorldDirection * Distance;
    FVector AimDirection = AimPoint - GetActorLocation();
    AimDirection.Z = 0.0f;

    return AimDirection.IsNearlyZero() ? GetActorForwardVector() : AimDirection.GetSafeNormal();
}

void ARiftPlayerCharacter::HandleDeath(URiftHealthComponent* DeadHealthComponent, AActor* DamageInstigator)
{
    if (!HasAuthority())
    {
        return;
    }

    GetCharacterMovement()->DisableMovement();

    if (VisualMesh)
    {
        VisualMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 0.18f));
    }

    if (ARiftGameMode* RiftGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ARiftGameMode>() : nullptr)
    {
        RiftGameMode->NotifyPlayerDied();
    }
}
