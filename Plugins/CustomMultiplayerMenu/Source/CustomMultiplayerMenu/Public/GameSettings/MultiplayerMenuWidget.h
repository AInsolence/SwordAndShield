// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSettings/MultiplayerMenuInterface.h"
#include "MultiplayerMenuWidget.generated.h"

USTRUCT()
struct FServerInfo
{
    GENERATED_BODY()
    
    FString ServerName;
    FString HostPlayerName;

    uint16 CurrentNumOfPlayers;
    uint16 MaxNumOfPlayers;
};

/**
 *
 */

UCLASS()
class CUSTOMMULTIPLAYERMENU_API UMultiplayerMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // Constructor
    UMultiplayerMenuWidget(const FObjectInitializer& ObjectInitializer);
    // Native constructor
    virtual void NativeConstruct() override;

    void SetMenuInterface(IMultiplayerMenuInterface* MenuInterfaceToSet);
    void SetServerSessionsList(const TArray<FServerInfo>& ServerInfoList);
    void SetSelectedSessionIndex(uint32 Index);

protected:

    UPROPERTY(EditAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UWidget* MainMultiplayerMenu;

    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UWidgetSwitcher* MultiplayerMenuSwitcher;

    // Host menu
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UWidget* HostMenu;
    UPROPERTY(EditAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UButton* HostButton;
    UPROPERTY(EditAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UButton* HostGameButton;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UButton* CancelHostMenuButton;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UEditableText* ServerNameEditableField;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UEditableText* MaxPlayersEditableField;

    // Join menu
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UWidget* JoinMenu;
    UPROPERTY(EditAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UButton* JoinButton;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UButton* JoinInnerMenuButton;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UScrollBox* ServerListScrollBox;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu")
    TSubclassOf<class UServerSessionRow> ServerSessionRowClass;
    UPROPERTY(EditAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UButton* CancelJoinMenuButton;

private:

    IMultiplayerMenuInterface* MenuInterface;

    // Host menu functions
    UFUNCTION(Category = "MainMultiplayerMenu")
        void HostOnClicked();
    UFUNCTION(Category = "MainMultiplayerMenu")
        void HostGameOnClicked();

    // Join menu functions
    UFUNCTION(Category = "MainMultiplayerMenu")
        void JoinOnClicked();
    UFUNCTION(Category = "MainMultiplayerMenu")
        void JoinInnerMenuButtonOnClicked();
    // For both menuse is the same cancel logic
    UFUNCTION(Category = "MainMultiplayerMenu")
        void CancelButtonOnClicked();
    //
    TOptional<uint32> SelectedSessionIndex;
    void UpdateChildrenSelectionInfo();
    uint16 DefaultMaxNumberOfPlayers = 10;
};
