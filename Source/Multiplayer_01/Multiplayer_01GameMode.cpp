// Copyright Epic Games, Inc. All Rights Reserved.

#include "Multiplayer_01GameMode.h"
#include "Multiplayer_01Character.h"
#include "UObject/ConstructorHelpers.h"

AMultiplayer_01GameMode::AMultiplayer_01GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
