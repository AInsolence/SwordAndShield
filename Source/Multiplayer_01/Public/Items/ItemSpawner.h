// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

UCLASS()
class MULTIPLAYER_01_API AItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//
	UClass* SpawnItemInRandomLocation(UClass* Item);
	UClass* SpawnItemInFrontOf(UClass* Item, float Distance);


private:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SpawnerVolume;

	// helper to find random location in volume
	FVector GetRandomPointInVolume();
};
