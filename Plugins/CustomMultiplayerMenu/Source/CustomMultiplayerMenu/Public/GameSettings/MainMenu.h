// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings/MultiplayerMenuWidget.h"
#include "GameSettings/MultiplayerMenuInterface.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMULTIPLAYERMENU_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
    // Constructor
    UMainMenu(const FObjectInitializer& ObjectInitializer);
    // Native constructor
    virtual void NativeConstruct() override;

    UPROPERTY(VisibleAnywhere, Category = "MainMenu", meta = (BindWidget))
    class UButton* MultiplayerButton;
    UFUNCTION(Category = "MainMenu")
    void MultiplayerButtonOnClicked();

    UPROPERTY(EditAnywhere, Category = "MainMenu")
    TSubclassOf<UMultiplayerMenuWidget> MultiplayerMenuWidgetClass;

    void SetMultiplayerMenuInterface(IMultiplayerMenuInterface* MenuInterfaceToSet);
    void SetServerSessionsList(const TArray<FServerInfo>& ServerInfoList);

private:
    
    UMultiplayerMenuWidget* MultiplayerMenuWidget;
};
