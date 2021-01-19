// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthBarPercentage(float HealthPoints);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;
};
