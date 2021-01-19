// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarWidget.h"
#include "Runtime/UMG/Public/UMG.h"

void UHealthBarWidget::SetHealthBarPercentage(float HealthPoints)
{
	UE_LOG(LogTemp, Warning, TEXT("Health bar updated = %f"), HealthPoints);
	HealthBar->SetPercent(HealthPoints);
}