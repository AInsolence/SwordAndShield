// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer_01Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/CombatComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/HealthComponent.h"


//////////////////////////////////////////////////////////////////////////
// AMultiplayer_01Character

AMultiplayer_01Character::AMultiplayer_01Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 0.f;
	GetCharacterMovement()->AirControl = 0;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create combat component
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>("EquipmentComponent");
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayer_01Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AMultiplayer_01Character::Sprint);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &AMultiplayer_01Character::Roll);
	PlayerInputComponent->BindAction("Attack01", IE_Pressed, this, &AMultiplayer_01Character::Attack01);
	PlayerInputComponent->BindAction("Attack02", IE_Pressed, this, &AMultiplayer_01Character::Attack02);
	PlayerInputComponent->BindAction("Block01", IE_Pressed, this, &AMultiplayer_01Character::Block01);
	PlayerInputComponent->BindAction("Block02", IE_Pressed, this, &AMultiplayer_01Character::Block02);
	PlayerInputComponent->BindAction("SwapWeapon", IE_Pressed, this, &AMultiplayer_01Character::SwapWeapon);


	PlayerInputComponent->BindAxis("MoveForward", this, &AMultiplayer_01Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMultiplayer_01Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMultiplayer_01Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMultiplayer_01Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMultiplayer_01Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMultiplayer_01Character::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMultiplayer_01Character::OnResetVR);
}

void AMultiplayer_01Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMultiplayer_01Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMultiplayer_01Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMultiplayer_01Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayer_01Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMultiplayer_01Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayer_01Character::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMultiplayer_01Character::Roll()
{
	UE_LOG(LogTemp, Warning, TEXT("Character Roll() called"))
	if (!CombatComponent || !HealthComponent)
	{
		return;
	}
	HealthComponent->SetIsSprinting(false);
	if (GetWorld()->GetTimeSeconds() < SprintRollPressedTime + 0.3f)
	{
		CombatComponent->Roll();
	}
	SprintRollPressedTime = 0.0f;
}

void AMultiplayer_01Character::Sprint()
{
	SprintRollPressedTime = GetWorld()->GetTimeSeconds();
	if (!HealthComponent)
	{
		return;
	}
	HealthComponent->SetIsSprinting(true);
}

void AMultiplayer_01Character::Attack01()
{
	if (!CombatComponent)
	{
		return;
	}
	CombatComponent->Attack01();
}

void AMultiplayer_01Character::Attack02()
{
	if (!CombatComponent)
	{
		return;
	}
	CombatComponent->Attack02();
}

void AMultiplayer_01Character::Block01()
{
	if (!CombatComponent)
	{
		return;
	}
	CombatComponent->Block01();
}

void AMultiplayer_01Character::Block02()
{
	if (!CombatComponent)
	{
		return;
	}
	CombatComponent->Block02();
}

void AMultiplayer_01Character::SwapWeapon()
{
	if (CombatComponent)
	{
		CombatComponent->SwapWeapon();
	}
}
