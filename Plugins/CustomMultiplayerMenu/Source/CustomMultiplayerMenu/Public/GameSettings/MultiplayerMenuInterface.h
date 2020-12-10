// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MultiplayerMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMultiplayerMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CUSTOMMULTIPLAYERMENU_API IMultiplayerMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Host(FString ServerName, uint16 MaxPlayersNumber) = 0;
	virtual void Join(uint32 SessionIndex) = 0;
	virtual void RefreshServerList() = 0;
	virtual void StartSession() = 0;
};
