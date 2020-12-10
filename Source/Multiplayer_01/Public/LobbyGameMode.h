// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Multiplayer_01GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API ALobbyGameMode : public AMultiplayer_01GameMode
{
	GENERATED_BODY()

protected:
	virtual void PostLogin(APlayerController* PlayerController) override;
	virtual void Logout(AController* PlayerController) override;

	FTimerHandle GameStartTimer;
	void StartSession();
};
