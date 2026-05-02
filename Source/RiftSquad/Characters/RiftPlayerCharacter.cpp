#include "Characters/RiftPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Combat/RiftHealthComponent.h"
#include "Combat/RiftWeaponComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

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
}

void ARiftPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

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

void ARiftPlayerCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();
    AddMovementInput(FVector::ForwardVector, MovementVector.Y);
    AddMovementInput(FVector::RightVector, MovementVector.X);
}

void ARiftPlayerCharacter::Fire(const FInputActionValue& Value)
{
    const FVector AimDirection = GetAimDirection();
    SetActorRotation(AimDirection.Rotation());

    if (WeaponComponent)
    {
        WeaponComponent->RequestFire(GetActorLocation(), AimDirection);
    }
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
