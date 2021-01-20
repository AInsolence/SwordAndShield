// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerState.h"
#include "Net/UnrealNetwork.h"

void ACustomPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACustomPlayerState, Deaths, COND_None);
}

int32 ACustomPlayerState::GetDeaths() const
{
	return Deaths;
}

void ACustomPlayerState::SetDeaths(int32 _Deaths)
{
	Deaths = _Deaths;
}