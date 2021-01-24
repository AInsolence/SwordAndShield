// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class provides information about players statistic e.g. kills, deaths,
	match states etc. by the player's request using the player's HUD. 

***/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatisticsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UStatisticsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatisticsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void ShowMatchStats();
	void HideMatchStats();

private:
	class ACharacter* Owner = nullptr;
	class AHUD_Multiplayer* GetPlayerHUD() const;
};
