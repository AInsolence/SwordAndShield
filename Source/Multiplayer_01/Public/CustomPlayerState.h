// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CustomPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	int32 GetDeaths() const;
	void SetDeaths(int32 _Deaths);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Deaths = 0;
};
