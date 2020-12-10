// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers += 1;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		ActiveTriggers -= 1;
	}
}

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	this->SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMovingPlatform, MoveSpeed);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Replication enable
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
	MoveDirection = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
}

void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// MOVE only on the server
	if (HasAuthority())
	{// check if the active trigger exists
		if (ActiveTriggers > 0)
		{
			// going to be a ZERO during the platform acheaving the target
			//MoveDirection = (GlobalTargetLocation - GetActorLocation()).GetSafeNormal();

			auto TotalDistance = FVector::Dist(GlobalTargetLocation, GlobalStartLocation);
			auto DistanceToCovered = FVector::Dist(GetActorLocation(), GlobalStartLocation);
			auto DistanceFromCovered = FVector::Dist(GetActorLocation(), GlobalTargetLocation);

			if (bIsPlatformGoForward)
			{
				SetActorLocation(GetActorLocation() + MoveSpeed * DeltaTime * MoveDirection);
				if (DistanceToCovered > TotalDistance)
				{
					bIsPlatformGoForward = false;
				}
			}
			else
			{
				SetActorLocation(GetActorLocation() + MoveSpeed * DeltaTime * MoveDirection * (-1));
				if (DistanceFromCovered > TotalDistance)
				{
					bIsPlatformGoForward = true;
				}
			}
		}
	}
}