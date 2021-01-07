// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUD_Multiplayer.h"

#include "HUD/PlayerStateInfoWidget.h"
#include "Kismet/GameplayStatics.h"

AHUD_Multiplayer::AHUD_Multiplayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AHUD_Multiplayer::BeginPlay()
{
	Super::BeginPlay();
	//*** NOTE ZOrder for created widgets depends on their order in this code ***//
	// add player state  widget
	if (PlayerStateInfoWidgetClass)
	{
		PlayerStateInfoWidget = CreateWidget<UPlayerStateInfoWidget>(GetWorld(), PlayerStateInfoWidgetClass);
		if (PlayerStateInfoWidget)
		{
			PlayerStateInfoWidget->AddToViewport();
			// Hide player status widget in main menu
			auto LevelName = GetWorld()->GetName();
			if (LevelName == "MainMenu")
			{
				PlayerStateInfoWidget->SetVisibility(ESlateVisibility::Hidden);
			}
			else // show in the game
			{
				PlayerStateInfoWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void AHUD_Multiplayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHUD_Multiplayer::DrawHUD()
{
	Super::DrawHUD();
}

void AHUD_Multiplayer::UpdateHealthState(float CurrentHealth)
{
	if (PlayerStateInfoWidget)
	{
		PlayerStateInfoWidget->UpdateHealthState(CurrentHealth);
	}
}

void AHUD_Multiplayer::UpdateStaminaState(float CurrentStamina)
{
	if (PlayerStateInfoWidget)
	{
		PlayerStateInfoWidget->UpdateStaminaState(CurrentStamina);
	}
}

void AHUD_Multiplayer::SetPickUpTipVisibility(bool Show)
{
	if (PlayerStateInfoWidget)
	{
		PlayerStateInfoWidget->SetPickUpTipVisibility(Show);
	}
}

void AHUD_Multiplayer::NotEnoughStamina()
{
	if (PlayerStateInfoWidget)
	{
		PlayerStateInfoWidget->NotEnoughStamina();
	}
}
