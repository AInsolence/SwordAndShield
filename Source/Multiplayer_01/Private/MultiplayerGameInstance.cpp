// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"
#include "GameSettings/MainMenu.h"
#include "GameSettings/MultiplayerMenuWidget.h"
#include "UObject/ConstructorHelpers.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UMultiplayerGameInstance::UMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetAsset
				(TEXT("/CustomMultiplayerMenu/CustomMenu/Widgets/MainMenuWidget"));
	if (ensure(MainMenuWidgetAsset.Class))
	{
		MainMenuClass = MainMenuWidgetAsset.Class;
		UE_LOG(LogTemp, Warning, TEXT("Menu class is found: %s"), *MainMenuWidgetAsset.Class)
	}
}

void UMultiplayerGameInstance::Init()
{
	Super::Init();

	// your other code ...
	auto Engine = GetEngine();
	if (!ensureMsgf(Engine, TEXT("Engine is null!"))) return;
	Engine->OnNetworkFailure().AddUObject(this, &UMultiplayerGameInstance::OnNetworkFailure);

	auto OnlineSubsystemInstance = IOnlineSubsystem::Get();
	if (OnlineSubsystemInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Online subsystem class name: %s"), *OnlineSubsystemInstance->GetInstanceName().ToString());
		OnlineSessionInstance = OnlineSubsystemInstance->GetSessionInterface();
		if (OnlineSessionInstance)
		{
			OnlineSessionInstance->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnCreateSessionComplete);
			OnlineSessionInstance->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnDestroySessionComplete);
			OnlineSessionInstance->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnFindSessionComplete);
			OnlineSessionInstance->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnJoinSessionComplete);
			OnlineSessionInstance->OnStartSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnStartSessionComplete);
		}
	}
}

void UMultiplayerGameInstance::Host(FString ServerNameToSet, uint16 MaxPlayersNumber, FString LevelName)
{
	if (OnlineSessionInstance.IsValid())
	{
		ServerName = ServerNameToSet;
		MaxNumberOfPlayers = MaxPlayersNumber;
		DefaultLevelName = LevelName;
		auto ExistingSession = OnlineSessionInstance->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			FOnDestroySessionCompleteDelegate RecreateSessionDelegate;
			RecreateSessionDelegate.BindUObject(this, &UMultiplayerGameInstance::RecreateSession);
			OnlineSessionInstance->DestroySession(NAME_GameSession, RecreateSessionDelegate);
		}
		else
		{
			CreateSession();
		}
	}
}

void UMultiplayerGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(!OnlineSessionInstance.IsValid()) return;
	
	FString SessionAddress;
	if (OnlineSessionInstance->GetResolvedConnectString(SessionName, SessionAddress))
	{
		auto PlayerController = GetFirstLocalPlayerController();
		if (ensure(PlayerController))
		{
			// Client Travel
			PlayerController->ClientTravel(SessionAddress, ETravelType::TRAVEL_Absolute);
			UE_LOG(LogTemp, Warning, TEXT("Client travel to: %s"), *SessionAddress);
			// Set input mode game only and hide mouse cursor in the game
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
		}
	}
}

void UMultiplayerGameInstance::CreateSession()
{
	if (OnlineSessionInstance.IsValid())
	{// created session settings (compulsory)
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{// For LAN
			SessionSettings.bIsLANMatch = true;
		}
		else
		{// For Steam
			SessionSettings.bIsLANMatch = false;
		}
		SessionSettings.bUsesPresence = true; // Need for STEAM
		SessionSettings.NumPublicConnections = MaxNumberOfPlayers;
		SessionSettings.bShouldAdvertise = true; // make session public
		SessionSettings.Set(FName("ServerName"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		OnlineSessionInstance->CreateSession(0, NAME_GameSession, SessionSettings);
	}
}

void UMultiplayerGameInstance::Join(uint32 SessionIndex)
{
	if (OnlineSessionInstance.IsValid() && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Try TO CONNECT"))
		OnlineSessionInstance->JoinSession(0, NAME_GameSession,
											SessionSearch->SearchResults[SessionIndex]);
	}
}

void UMultiplayerGameInstance::RefreshServerList()
{
	if (!OnlineSessionInstance.IsValid()) return;

	///* Find session *///
	SessionSearch = MakeShareable(new FOnlineSessionSearch()); // Create session search object
	if (SessionSearch.IsValid())
	{// set session search query parameters
		SessionSearch->bIsLanQuery = false;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		OnlineSessionInstance->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UMultiplayerGameInstance::StartSession()
{
	if (OnlineSessionInstance.IsValid())
	{
		OnlineSessionInstance->StartSession(NAME_GameSession);
	}
}

void UMultiplayerGameInstance::OnStartSessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session is created SUCCESSFULLY"))
		RefreshServerList();
	}
}

void UMultiplayerGameInstance::RecreateSession(FName SessionName, bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session is recreated"))
		CreateSession();
	}
}

void UMultiplayerGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& Message)
{
	// Travel to main menu
	auto* PlayerController = GetFirstLocalPlayerController();

	if (!ensureMsgf(PlayerController, TEXT("Couldn't get PlayerController object!"))) return;
	PlayerController->ClientTravel("/CustomMultiplayerMenu/CustomMenu/MainMenu", ETravelType::TRAVEL_Absolute);
	// Destroy session
	if (!ensureMsgf(OnlineSessionInstance.IsValid(), TEXT("Session interface is invalid!"))) return;
	const auto* ExistingSession = OnlineSessionInstance->GetNamedSession(NAME_GameSession);
	if (ExistingSession) OnlineSessionInstance->DestroySession(NAME_GameSession);
}

void UMultiplayerGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game instance could not create session."))
		return;
	}
	UWorld* World = GetWorld();
	if (ensure(World))
	{
		World->ServerTravel("/Game/Maps/" + DefaultLevelName + "?listen");
		auto PlayerController = GetFirstLocalPlayerController(World);
		if (ensure(PlayerController))
		{// Set input mode game only and hide mouse cursor in the game
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
		}
	}
}

void UMultiplayerGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("Session is destroyed"))
}

void UMultiplayerGameInstance::OnFindSessionComplete(bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sessions was found"))
			if (SessionSearch)
			{
				if (ensure(MainMenu))
				{
					auto SearchResult = SessionSearch->SearchResults;
					if (SearchResult.Num() > 0)
					{
						TArray<FServerInfo> SessionsInfoList;
						for (auto Result : SearchResult)
						{// create server info
							FServerInfo ServerData;
							// set server info
							Result.Session.SessionSettings.Get(FName("ServerName"), ServerName);
							ServerData.ServerName = ServerName;
							ServerData.HostPlayerName = Result.Session.OwningUserName;
							ServerData.CurrentNumOfPlayers = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;
							ServerData.MaxNumOfPlayers = Result.Session.SessionSettings.NumPublicConnections;

							SessionsInfoList.Add(ServerData);
						}
						MainMenu->SetServerSessionsList(SessionsInfoList);
					}
				}
			}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Session is NOT found"))
	}
}

void UMultiplayerGameInstance::CreateMainMenu()
{
	if (ensure(MainMenuClass))
	{
		// Create main menu
		UUserWidget* MenuWidget = CreateWidget(this, MainMenuClass);
		if (ensure(MenuWidget))
		{// set widget properties if the widget was created
			MenuWidget->AddToViewport();
			MenuWidget->SetVisibility(ESlateVisibility::Visible);
			// Inject menu interface dependency
			MainMenu = Cast<UMainMenu>(MenuWidget);
			if (ensure(MainMenu))
			{
				UE_LOG(LogTemp, Warning, TEXT("Main menu after casting"))
				MainMenu->SetMultiplayerMenuInterface(this);
			}
		}
	}
}

void UMultiplayerGameInstance::LoadMainMenu()
{
	UWorld* World = GetWorld();
	if (ensure(World))
	{
		auto PlayerController = GetFirstLocalPlayerController(World);
		if (ensure(PlayerController))
		{// Client Travel to main menu
			PlayerController->ClientTravel("MainMenu", ETravelType::TRAVEL_Absolute);
			// Set input mode game only and hide mouse cursor in the game
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;
		}
	}
}
