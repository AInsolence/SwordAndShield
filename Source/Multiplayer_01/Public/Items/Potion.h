// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Potion.generated.h"

UCLASS()
class MULTIPLAYER_01_API APotion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APotion();
	// Called every frame
	void Tick(float DeltaTime) override;

	/** Called when a weapon hits something */
	UFUNCTION(BlueprintCallable, Category = "Potion")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Potion")
	class UBoxComponent* PickUpCollisionBox = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Potion")
	class UStaticMeshComponent* PotionMesh = nullptr;

	// Potion stats

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Component")
	class USoundBase* PickUpSoundFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PotionStats")
	int32 HealthPoints = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PotionStats")
	int32 ManaPoints = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PotionStats")
	int32 StaminaPoints = 0;

	// Rotation stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotationStats")
	float Roll = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotationStats")
	float Pitch = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotationStats")
	float Yaw = 0.0;

private:
	void PotionAnimation();
};
