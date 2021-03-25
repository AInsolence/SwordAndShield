// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ManaComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HUD_Multiplayer.h"

// Sets default values for this component's properties
UManaComponent::UManaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	//
	CurrentMana = DefaultMana;
}

void UManaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UManaComponent, CurrentMana, COND_OwnerOnly);
}

// Called when the game starts
void UManaComponent::BeginPlay()
{
	Super::BeginPlay();
	// Set owner reference
	Owner = Cast<ACharacter>(GetOwner());
}

void UManaComponent::NotEnoughMana()
{
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->NotEnoughMana();
	}
}

void UManaComponent::OnRep_StateChanged()
{
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->UpdateManaState(GetCurrentMana() / DefaultMana);
	}
}

void UManaComponent::ChangeCurrentManaTo(float ManaCost)
{
	if (!Owner->HasAuthority())
	{
		return;
	}
	CurrentMana = FMath::Clamp(CurrentMana + ManaCost, 0.0f, DefaultMana);
	// Change mana on listen-server client HUD
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->UpdateManaState(GetCurrentMana() / DefaultMana);
	}
}

AHUD_Multiplayer* UManaComponent::GetPlayerHUD()
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
