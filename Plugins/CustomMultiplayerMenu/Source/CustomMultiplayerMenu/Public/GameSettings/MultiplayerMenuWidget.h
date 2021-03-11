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

    UPROPERTY(VisibleAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UWidget* MainMultiplayerMenu;

    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UWidgetSwitcher* MultiplayerMenuSwitcher;

    // Host menu
    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UWidget* HostMenu;
    UPROPERTY(VisibleAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UButton* HostButton;
    UPROPERTY(VisibleAnywhere, Category = "HostMultiplayerMenu", meta = (BindWidget))
    class UButton* HostGameButton;
    UPROPERTY(VisibleAnywhere, Category = "HostMultiplayerMenu", meta = (BindWidget))
    class UButton* CancelHostMenuButton;
    UPROPERTY(VisibleAnywhere, Category = "HostMultiplayerMenu", meta = (BindWidget))
    class UEditableText* ServerNameEditableField;
    UPROPERTY(VisibleAnywhere, Category = "HostMultiplayerMenu", meta = (BindWidget))
    class UEditableText* MaxPlayersEditableField;
	UPROPERTY(VisibleAnywhere, Category = "HostMultiplayerMenu", meta = (BindWidget))
	class UComboBoxString* LevelsComboBox;
	UPROPERTY(VisibleAnywhere, Category = "HostMultiplayerMenu", meta = (BindWidget))
	class UImage* Level_Image;

	UPROPERTY(EditAnywhere, Category = "HostMultiplayerMenu")
	class UTexture2D* LevelImgPlaceholder;
	UPROPERTY(EditAnywhere, Category = "HostMultiplayerMenu")
	TMap<FName, UTexture2D*> LevelsToHost;
	UPROPERTY(EditAnywhere, Category = "HostMultiplayerMenu")
	FString DefaultLevel = "DefaultLevel";

    // Join menu
    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UWidget* JoinMenu;
    UPROPERTY(VisibleAnywhere, Category = "MainMultiplayerMenu", meta = (BindWidget))
    class UButton* JoinButton;
    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UButton* JoinInnerMenuButton;
    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UScrollBox* ServerListScrollBox;
    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu")
    TSubclassOf<class UServerSessionRow> ServerSessionRowClass;
    UPROPERTY(VisibleAnywhere, Category = "JoinMultiplayerMenu", meta = (BindWidget))
    class UButton* CancelJoinMenuButton;

private:

    IMultiplayerMenuInterface* MenuInterface;

    // Host menu functions
    UFUNCTION(Category = "MainMultiplayerMenu")
    void HostOnClicked();
    UFUNCTION(Category = "MainMultiplayerMenu")
    void HostGameOnClicked();
    UFUNCTION(Category = "MainMultiplayerMenu")
    void ChangeLevelImage(FString SelectedItem, ESelectInfo::Type SelectionType);

    // Join menu functions
    UFUNCTION(Category = "MainMultiplayerMenu")
    void JoinOnClicked();
    UFUNCTION(Category = "MainMultiplayerMenu")
    void JoinInnerMenuButtonOnClicked();
    // For both menus is the same cancel logic
    UFUNCTION(Category = "MainMultiplayerMenu")
    void CancelButtonOnClicked();
    //
    TOptional<uint32> SelectedSessionIndex;
    void UpdateChildrenSelectionInfo();
    uint16 DefaultMaxNumberOfPlayers = 10;
};
