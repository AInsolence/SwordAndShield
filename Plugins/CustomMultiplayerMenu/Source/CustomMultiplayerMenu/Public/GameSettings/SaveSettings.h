// Copyright © 2020 Insolence Assets, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Containers/Map.h"
#include "SaveSettings.generated.h"

/**
	Class manages the settings save data
 */

UCLASS()
class CUSTOMMULTIPLAYERMENU_API USaveSettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveSettings();

	UPROPERTY(VisibleAnywhere, Category = "Save Settings")
	uint32 UserIndex = 0;
	UPROPERTY(VisibleAnywhere, Category = "Save Settings")
	FString SettingsSlot;
	UPROPERTY(VisibleAnywhere, Category = "Save Settings")
	TMap<FString, FString> SettingsValues;
};
