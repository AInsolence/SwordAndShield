// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSettings/MainMenu.h"
#include "Components/Button.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{// Find multiplayer widget class
	static ConstructorHelpers::FClassFinder<UUserWidget> MultiplayerMenuWidgetAsset
				(TEXT("/CustomMultiplayerMenu/CustomMenu/Widgets/MultiplayerWidget"));
	if (ensure(MultiplayerMenuWidgetAsset.Class))
	{
		MultiplayerMenuWidgetClass = MultiplayerMenuWidgetAsset.Class;
		UE_LOG(LogTemp, Warning, TEXT("Multiplayer class is found: %s"), *MultiplayerMenuWidgetAsset.Class)
	}
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	if (ensure(MultiplayerButton))
	{// Connect OnClick event
		MultiplayerButton->OnClicked.
		AddDynamic(this, &UMainMenu::MultiplayerButtonOnClicked);
	}
}

void UMainMenu::MultiplayerButtonOnClicked()
{
	if (ensure(MultiplayerMenuWidget))
	{// show widget if exists
		MultiplayerMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainMenu::SetMultiplayerMenuInterface(IMultiplayerMenuInterface* MenuInterfaceToSet)
{
	// if class was found -> create multiplayer menu widget
	if (ensure(MultiplayerMenuWidgetClass))
	{// Create multiplayer menu widget
		auto MultiplayerMenuUserWidget = CreateWidget(this, MultiplayerMenuWidgetClass);
		if (ensure(MultiplayerMenuUserWidget))
		{
			MultiplayerMenuWidget = Cast<UMultiplayerMenuWidget>(MultiplayerMenuUserWidget);
			if (ensure(MultiplayerMenuWidget))
			{// set widget properties if the widget was created
				MultiplayerMenuWidget->AddToViewport();
				MultiplayerMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	if (ensure(MultiplayerMenuWidget) && ensure(MenuInterfaceToSet))
	{
		UE_LOG(LogTemp, Warning, TEXT("INSIDE the set multiplayer menu"))
		MultiplayerMenuWidget->SetMenuInterface(MenuInterfaceToSet);
	}
}

void UMainMenu::SetServerSessionsList(const TArray<FServerInfo>& ServerInfoList)
{
	if (ensure(MultiplayerMenuWidget))
	{// set session list in multiplayer menu widget
		MultiplayerMenuWidget->SetServerSessionsList(ServerInfoList);
	}
}
