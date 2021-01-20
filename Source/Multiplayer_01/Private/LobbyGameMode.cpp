// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "TimerManager.h"
#include "MultiplayerGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* PlayerController)
{
	Super::PostLogin(PlayerController);

	auto NumOfPlayers = GetNumPlayers();
	UE_LOG(LogTemp, Warning, TEXT("Players at server = %d"), NumOfPlayers);
	//
	/*if (NumOfPlayers == 5)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartSession, 5.f);
	}*/
}

void ALobbyGameMode::Logout(AController* PlayerController)
{
	Super::Logout(PlayerController);

	auto NumOfPlayers = GetNumPlayers();
	UE_LOG(LogTemp, Warning, TEXT("Players at server = %d"), NumOfPlayers);
}

void ALobbyGameMode::StartSession()
{
	auto GameInstance = Cast<UMultiplayerGameInstance>(GetGameInstance());
	if(GameInstance == nullptr) return;

	GameInstance->StartSession();
	UWorld* World = GetWorld();
	if (ensure(World))
	{
		bUseSeamlessTravel = true;
		World->ServerTravel("/Game/Maps/Level_01?listen");
	}
}
