// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameSettings/MultiplayerMenuInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerGameInstance.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYER_01_API UMultiplayerGameInstance : public UGameInstance, public IMultiplayerMenuInterface
{
	GENERATED_BODY()
	
public:
	UMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;

	// Menu interface implementation
	UFUNCTION(Exec, Category = "Multiplayer")
	virtual void Host(FString ServerNameToSet, uint16 MaxPlayersNumber, FString LevelName) override;
	UFUNCTION(Exec, Category = "Multiplayer")
	virtual void Join(uint32 SessionIndex) override;
	UFUNCTION(Exec, Category = "Multiplayer")
	virtual void RefreshServerList() override;
	UFUNCTION(Exec, Category = "Multiplayer")
	virtual void StartSession() override;
	//
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void CreateMainMenu();
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void LoadMainMenu();

	UPROPERTY(EditAnywhere, Category = "Multiplayer")
	TSubclassOf<class UMainMenu> MainMenuClass;
	UPROPERTY(BlueprintReadWrite, Category = "Multiplayer")
	UMainMenu* MainMenu = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Multiplayer")
	FString DefaultLevelName = "DefaultLevel";

	// Online session related data and methods
	IOnlineSessionPtr OnlineSessionInstance;
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void CreateSession();
	FString ServerName = "Default Server Name";
	uint16 MaxNumberOfPlayers = 10;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionComplete(FName SessionName, bool Success);
	void RecreateSession(FName SessionName, bool Success);

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void OnNetworkFailure(UWorld* World, 
							UNetDriver* NetDriver, 
							ENetworkFailure::Type FailureType, 
							const FString& Message);

	// Session search object using in a Find session process
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
	//
};
