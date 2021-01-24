// Copyright © 2021 Insolence Assets. All Rights Reserved.

#include "Components/StatisticsComponent.h"

#include "HUD/HUD_Multiplayer.h"
#include "GameFramework/Character.h"
#include "Components/HealthComponent.h"
#include "CustomPlayerState.h"

// Sets default values for this component's properties
UStatisticsComponent::UStatisticsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

// Called when the game starts
void UStatisticsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set owner reference
	Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		// Set combat component to call animations
		auto HealthCompObject = GetOwner()->GetDefaultSubobjectByName("HealthComponent");
		if (HealthCompObject)
		{
			HealthComponent = Cast<UHealthComponent>(HealthCompObject);
			if (HealthComponent)
			{
				HealthComponent->DeathEvent.AddDynamic(this, &UStatisticsComponent::SetScores);
			}
		}
	}
}

void UStatisticsComponent::ShowMatchStats()
{
	auto PlayerHUD = GetPlayerHUD();
	if (PlayerHUD)
	{
		PlayerHUD->ShowMatchStats();
	}
}

void UStatisticsComponent::HideMatchStats()
{
	auto PlayerHUD = GetPlayerHUD();
	if (PlayerHUD)
	{
		PlayerHUD->HideMatchStats();
	}
}

AHUD_Multiplayer* UStatisticsComponent::GetPlayerHUD() const
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

void UStatisticsComponent::SetScores(AController* InstigatedBy)
{
	auto KilledBy = InstigatedBy->GetPlayerState<APlayerState>();
	if (KilledBy)
	{
		KilledBy->SetScore(KilledBy->GetScore() + 1);
	}
	auto PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController)
	{
		auto PlayerState = Cast<ACustomPlayerState>(PlayerController->GetPlayerState<APlayerState>());
		if (PlayerState)
		{
			PlayerState->SetDeaths(PlayerState->GetDeaths() + 1);
		}
	}
	auto PlayerHUD = GetPlayerHUD();
	if (PlayerHUD)
	{
		PlayerHUD->UpdateScoreTable();
	}
}
