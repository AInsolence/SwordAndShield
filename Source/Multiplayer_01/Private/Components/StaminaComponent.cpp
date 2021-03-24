// Copyright © 2021 Insolence Assets. All Rights Reserved.

#include "Components/StaminaComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HUD_Multiplayer.h"

// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	//
	StaminaServerState.CurrentStamina = DefaultStamina;
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UStaminaComponent, StaminaServerState, COND_OwnerOnly);
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	// Set owner reference
	Owner = Cast<ACharacter>(GetOwner());
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/**  Sprint logic server-side*/
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}
	//
	float RampThisFrame = (DeltaTime / TimeToMaxSprintSpeed) * MaxSprintMultiplier;
	// check if sprint input pressed, is character has enough stamina and is he moves
	if (StaminaServerState.bIsSprinting && !Owner->GetVelocity().IsZero() &&
		GetCurrentStamina() > 0.5f)
	{
		BaseSprintMultiplier += RampThisFrame;
		// change stamina bar percentage
		ChangeCurrentStaminaTo(-25.0f * DeltaTime);
	}
	else
	{	
		BaseSprintMultiplier -= RampThisFrame;
		
		if (StaminaServerState.CurrentStamina == DefaultStamina)
		{// Stamina full
			return;
		}
		else
		{// restore stamina
			ChangeCurrentStaminaTo(15.0f * DeltaTime);
		}
	}
	BaseSprintMultiplier = FMath::Clamp(BaseSprintMultiplier, 1.0f, MaxSprintMultiplier);
	Owner->GetCharacterMovement()->MaxWalkSpeed = BaseWalkingSpeed * BaseSprintMultiplier;
	/**  Sprint logic end */
}

void UStaminaComponent::ChangeCurrentStaminaTo(float StaminaCost)
{
	if (Owner->HasAuthority())
	{
		StaminaServerState.CurrentStamina = FMath::Clamp(StaminaServerState.CurrentStamina + StaminaCost, 0.0f, DefaultStamina);
		// Change stamina on listen-server client HUD
		if (GetPlayerHUD() != nullptr)
		{
			GetPlayerHUD()->UpdateStaminaState(GetCurrentStamina() / GetDefaultStamina());
		}
	}
}

void UStaminaComponent::NotEnoughStamina()
{
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->NotEnoughStamina();
	}
}

void UStaminaComponent::SetIsSprinting(bool IsSprinting)
{
	Server_ChangeState(IsSprinting);
}

void UStaminaComponent::OnRep_StateChanged()
{
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->UpdateStaminaState(GetCurrentStamina() / GetDefaultStamina());
	}
}

void UStaminaComponent::Server_ChangeState_Implementation(bool IsSprinting)
{
	StaminaServerState.bIsSprinting = IsSprinting;
}

AHUD_Multiplayer* UStaminaComponent::GetPlayerHUD()
{
	if (Owner)
	{
		// We only want locally controlled and controlled by player, not AI
		if (!Owner->IsLocallyControlled())
		{
			return nullptr;
		}

		auto PlayerController = Cast<APlayerController>(Owner->GetController());
		if (!PlayerController)
		{
			return nullptr;
		}
		auto HUD = PlayerController->GetHUD();
		return Cast<AHUD_Multiplayer>(HUD);
	}
	return nullptr;
}
