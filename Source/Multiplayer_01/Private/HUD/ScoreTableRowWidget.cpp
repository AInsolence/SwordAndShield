// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ScoreTableRowWidget.h"
#include "Components/TextBlock.h"

void UScoreTableRowWidget::SetData(FString _PlayerName, int32 _Scores, int32 _Deaths)
{
	PlayerName->SetText(FText::FromString(_PlayerName));
	Scores->SetText(FText::AsNumber(_Scores));
	Deaths->SetText(FText::AsNumber(_Deaths));
}

void UScoreTableRowWidget::SetColor(FColor Color)
{
	PlayerName->SetColorAndOpacity(FSlateColor(Color));
	Scores->SetColorAndOpacity(FSlateColor(Color));
	Deaths->SetColorAndOpacity(FSlateColor(Color));
}
