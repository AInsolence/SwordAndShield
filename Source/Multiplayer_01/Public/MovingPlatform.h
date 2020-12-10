// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API AMovingPlatform : public AStaticMeshActor
{
public:	

	GENERATED_BODY()

	void AddActiveTrigger();
	void RemoveActiveTrigger();

protected:
	AMovingPlatform();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;

	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = true))
	FVector TargetLocation;
	FVector MoveDirection;

	UPROPERTY(EditAnywhere, Replicated)
	float MoveSpeed = 50.f;
	bool bIsPlatformGoForward = true;

	UPROPERTY(EditAnywhere)
	int ActiveTriggers = 1;
};
