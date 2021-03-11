// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSettings/MultiplayerMenuWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"

#include "GameSettings/ServerSessionRow.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"

#include "Misc/DefaultValueHelper.h"

UMultiplayerMenuWidget::UMultiplayerMenuWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	// Find server session row widget class
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerSessionRowAsset
	(TEXT("/CustomMultiplayerMenu/CustomMenu/Widgets/WBP_ServerSessionRow"));
	if (ensure(ServerSessionRowAsset.Class))
	{
		ServerSessionRowClass = ServerSessionRowAsset.Class;
		UE_LOG(LogTemp, Warning, TEXT("Server session row class is found: %s"), *ServerSessionRowClass)
	}
}

void UMultiplayerMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Join menu bindings
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, 
				&UMultiplayerMenuWidget::HostOnClicked);
	}
	if (HostGameButton)
	{
		HostGameButton->OnClicked.AddDynamic(this,
			&UMultiplayerMenuWidget::HostGameOnClicked);
	}
	// Join menu bindings
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, 
				&UMultiplayerMenuWidget::JoinOnClicked);
	}
	if (JoinInnerMenuButton)
	{
		JoinInnerMenuButton->OnClicked.AddDynamic(this,
					&UMultiplayerMenuWidget::JoinInnerMenuButtonOnClicked);
	}
	if (CancelHostMenuButton)
	{
		CancelHostMenuButton->OnClicked.AddDynamic(this,
			&UMultiplayerMenuWidget::CancelButtonOnClicked);
	}
	if (CancelJoinMenuButton)
	{
		CancelJoinMenuButton->OnClicked.AddDynamic(this,
			&UMultiplayerMenuWidget::CancelButtonOnClicked);
	}
	if(LevelsComboBox)
	{
		LevelsComboBox->OnSelectionChanged.AddDynamic(this, 
				&UMultiplayerMenuWidget::ChangeLevelImage);
		if (LevelsToHost.Num() > 0)
		{
			for (auto level : LevelsToHost)
			{
				LevelsComboBox->AddOption(level.Key.ToString());
			}
			LevelsComboBox->SetSelectedOption(DefaultLevel);
		}
	}
}

void UMultiplayerMenuWidget::HostGameOnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Host button clicked"))
	if (ensure(MenuInterface))
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerMenu.cpp: Host interface started"))
		if (ensure(ServerNameEditableField) && ensure(MaxPlayersEditableField))
		{
			auto ServerName = ServerNameEditableField->GetText().ToString();
			auto NumOfPlayersString = MaxPlayersEditableField->GetText().ToString();
			auto LevelName = LevelsComboBox->GetSelectedOption();
			if (LevelName.IsEmpty())
			{
				LevelName = "Barbican";
			}
			int32 OutNumOfPlayers;
			if(FDefaultValueHelper::ParseInt(NumOfPlayersString, OutNumOfPlayers))
			{
				MenuInterface->Host(ServerName, OutNumOfPlayers, LevelName);
			}
			else
			{// call with default max number of players
				MenuInterface->Host(ServerName, DefaultMaxNumberOfPlayers, LevelName);
			}
			
		}
	}
}

void UMultiplayerMenuWidget::ChangeLevelImage(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	auto LevelImg = *LevelsToHost.Find(FName(SelectedItem));
	if (LevelImg)
	{
		Level_Image->SetBrushFromTexture(LevelImg);
	}
	else if (LevelImgPlaceholder)
	{
		Level_Image->SetBrushFromTexture(LevelImgPlaceholder);
	}
}

void UMultiplayerMenuWidget::HostOnClicked()
{
	if (ensure(MultiplayerMenuSwitcher))
	{
		if (ensure(HostMenu))
		{
			UE_LOG(LogTemp, Warning, TEXT("Host menu exists"))
			MultiplayerMenuSwitcher->SetActiveWidget(HostMenu);
		}
	}
}

void UMultiplayerMenuWidget::JoinOnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join button is clicked"))
	if (ensure(MultiplayerMenuSwitcher))
	{
		if (ensure(JoinMenu))
		{
			UE_LOG(LogTemp, Warning, TEXT("Join menu exists"))
			MultiplayerMenuSwitcher->SetActiveWidget(JoinMenu);
			if (ensure(MenuInterface))
			{
				MenuInterface->RefreshServerList();
			}
		}
	}
}

void UMultiplayerMenuWidget::JoinInnerMenuButtonOnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join INNER button is clicked"))
	if (ensure(MenuInterface))
	{
		if (ensure(ServerListScrollBox))
		{
			if (SelectedSessionIndex.IsSet())
			{
				MenuInterface->Join(SelectedSessionIndex.GetValue());
				UE_LOG(LogTemp, Warning, TEXT("Try to connect to session = %d"), SelectedSessionIndex.GetValue());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Session index doesn't set"));
			}
		}
	}
}

void UMultiplayerMenuWidget::CancelButtonOnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Cancel join menu button clicked"))
		if (ensure(MultiplayerMenuSwitcher))
		{
			if (ensure(CancelJoinMenuButton))
			{
				MultiplayerMenuSwitcher->SetActiveWidget(MainMultiplayerMenu);
			}
		}
}

void UMultiplayerMenuWidget::SetMenuInterface(IMultiplayerMenuInterface* MenuInterfaceToSet)
{
	if (ensure(MenuInterfaceToSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("MultiplayerMenu.cpp: menu interface is set"))
		MenuInterface = MenuInterfaceToSet;
	}
}

void UMultiplayerMenuWidget::SetServerSessionsList(const TArray<FServerInfo>& ServerInfoList)
{
	if (ensure(ServerListScrollBox))
	{
		if (ensure(ServerSessionRowClass))
		{// delete old list before refreshing
			ServerListScrollBox->ClearChildren();
			uint32 SessionIndex = 0;
			for (const auto& SessionInfoString : ServerInfoList)
			{
				// Create multiplayer menu widget
				auto SessionRow = CreateWidget<UServerSessionRow>(this, ServerSessionRowClass);
				// Set server info
				SessionRow->SessionName->SetText(FText::FromString(SessionInfoString.ServerName));
				SessionRow->HostPlayerName->SetText(FText::FromString(SessionInfoString.HostPlayerName));
				FString PlayersCount = FString::FromInt(SessionInfoString.CurrentNumOfPlayers) + "/" +
														FString::FromInt(SessionInfoString.MaxNumOfPlayers);
				SessionRow->PlayersCount->SetText(FText::FromString(PlayersCount));
				//
				SessionRow->Setup(this, SessionIndex);
				++SessionIndex;
				ServerListScrollBox->AddChild(SessionRow);
			}
		}
	}
}

void UMultiplayerMenuWidget::SetSelectedSessionIndex(uint32 Index)
{
	SelectedSessionIndex = Index;
	UpdateChildrenSelectionInfo();
}

void UMultiplayerMenuWidget::UpdateChildrenSelectionInfo()
{
	for (auto Child : ServerListScrollBox->GetAllChildren())
	{
		auto SessionRow = Cast<UServerSessionRow>(Child);
		if (SessionRow)
		{
			bool RowSelectionStatus = (SessionRow->GetSessionIndex() == SelectedSessionIndex);
			SessionRow->SetAsSelected(RowSelectionStatus);
		}
	}
}
