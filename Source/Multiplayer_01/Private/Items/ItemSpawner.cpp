// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemSpawner.h"
#include "Components\BoxComponent.h"
#include "Kismet\KismetMathLibrary.h"

// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	// create spawner volume box
	SpawnerVolume = CreateDefaultSubobject<UBoxComponent>("SpawnerVolume");
	SpawnerVolume->SetRelativeScale3D(FVector(5, 5, 0));
	RootComponent = SpawnerVolume;
}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
}

UClass* AItemSpawner::SpawnItemInRandomLocation(UClass* Item)
{
	if (HasAuthority())
	{
		FVector Location = GetRandomPointInVolume();
		auto SpawnedItem = GetWorld()->SpawnActor<AActor>(Item, Location, FRotator::ZeroRotator);
		return SpawnedItem->GetClass();
	}
	else
	{
		return nullptr;
	}
}

UClass* AItemSpawner::SpawnItemInFrontOf(UClass* Item, float Distance)
{
	if (HasAuthority())
	{
		FVector Location = GetActorLocation() + GetActorForwardVector() * Distance;
		auto SpawnedItem = GetWorld()->SpawnActor<AActor>(Item, Location, FRotator::ZeroRotator);
		return SpawnedItem->GetClass();
	}
	else
	{
		return nullptr;
	}
}

FVector AItemSpawner::GetRandomPointInVolume()
{
	FVector OriginOfVolume = SpawnerVolume->Bounds.Origin;
	FVector ExtentOfVector = SpawnerVolume->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(OriginOfVolume, ExtentOfVector);
}