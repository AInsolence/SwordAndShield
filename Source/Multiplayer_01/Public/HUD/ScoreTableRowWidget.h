// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreTableRowWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API UScoreTableRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetData(FString PlayerName, int32 Scores, int32 Deaths);
	void SetColor(FColor Color);

protected:
	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UTextBlock* PlayerName;
	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UTextBlock* Scores;
	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UTextBlock* Deaths;
};
