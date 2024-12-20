// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformTrigger.generated.h"

class AMovingPlatform;

UCLASS()
class MULTIPLAYER_01_API APlatformTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerVolume;
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* PressurePad;
	
	UPROPERTY(EditAnywhere, Category = "PlatformsToTrigger")
	TArray<AMovingPlatform*> PlatformsToTrigger;

	UFUNCTION()
	void OnOverlapTriggerBegin(UPrimitiveComponent* OverlappedComponent,
								AActor* OtherActor,
								UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex,
								bool bFromSweep, 
								const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapTriggerEnd(UPrimitiveComponent* OverlappedComponent, 
								AActor* OtherActor,
								UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex);
};
