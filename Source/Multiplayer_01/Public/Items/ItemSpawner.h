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
	// API
	UClass* SpawnItemInRandomLocation(UClass* Item);
	UClass* SpawnItemInFrontOf(UClass* Item, float Distance);
	UFUNCTION(BlueprintCallable, Category = "ItemSpawner")
	void SpawnActorByTimer(UClass* Item);
	//
	AActor* ActorSpawned = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawnByTimer;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Spawning")
	float TimeStep = 0.0;

private:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SpawnerVolume;

	// helper to find random location in volume
	FVector GetRandomPointInVolume();

	FTimerHandle TimerHandler;
	FTimerDelegate TimerDelegate;
};
