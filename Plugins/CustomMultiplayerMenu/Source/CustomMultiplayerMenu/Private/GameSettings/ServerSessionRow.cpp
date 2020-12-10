// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSettings/ServerSessionRow.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameSettings/MultiplayerMenuWidget.h"

void UServerSessionRow::OnSessionRowButtonClicked()
{
	Parent->SetSelectedSessionIndex(SessionIndex);
	UE_LOG(LogTemp, Warning, TEXT("Session index = %d"), SessionIndex);
}


void UServerSessionRow::SetRowTextColor(FLinearColor Color)
{
	SessionName->SetColorAndOpacity(Color);
	HostPlayerName->SetColorAndOpacity(Color);
	PlayersCount->SetColorAndOpacity(Color);
}

void UServerSessionRow::SetAsSelected(bool IsSelected)
{
	if (IsSelected)
	{
		UnhoveredColor = FLinearColor::Blue;
		HoveredColor = FLinearColor::Green;
		SetRowTextColor(UnhoveredColor);
	}
	else
	{
		UnhoveredColor = FLinearColor::White;
		HoveredColor = FLinearColor::Yellow;
		SetRowTextColor(UnhoveredColor);
	}
}

void UServerSessionRow::OnSessionRowButtonOnHovered()
{
	SetRowTextColor(HoveredColor);
}

void UServerSessionRow::OnSessionRowButtonOnUnhovered()
{
	SetRowTextColor(UnhoveredColor);
}

void UServerSessionRow::Setup(UMultiplayerMenuWidget* ParentToSet, uint32 SessionIndexToSet)
{
	Parent = ParentToSet;
	SessionIndex = SessionIndexToSet;
	SessionRowButton->OnClicked.AddDynamic(this, &UServerSessionRow::OnSessionRowButtonClicked);
	SessionRowButton->OnHovered.AddDynamic(this, &UServerSessionRow::OnSessionRowButtonOnHovered);
	SessionRowButton->OnUnhovered.AddDynamic(this, &UServerSessionRow::OnSessionRowButtonOnUnhovered);
}
