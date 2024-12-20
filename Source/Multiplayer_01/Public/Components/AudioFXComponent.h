// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioFXComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UAudioFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAudioFXComponent();

	// API
	UFUNCTION(Server, Unreliable, BlueprintCallable, Category = "Audio Component")
	void Server_PlaySoundFX(USoundBase* SoundFX);
	void Server_PlaySoundFX_Implementation(USoundBase* SoundFX);
	//
	UFUNCTION(NetMulticast, Unreliable, Category = "Audio Component")
	void Multicast_PlaySoundFX(USoundBase* SoundFX);
	void Multicast_PlaySoundFX_Implementation(USoundBase* SoundFX);
	//
	UFUNCTION(BlueprintCallable, Category = "Audio Component")
	void Blocked();
	UFUNCTION(BlueprintCallable, Category = "Audio Component")
	void PickUp();
	UFUNCTION(BlueprintCallable, Category = "Audio Component")
	void Death(AController* InstigatedBy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Component")
	class USoundBase* Roll_AudioFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Component")
	class USoundBase* EquipItem_AudioFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Component")
	class USoundBase* Death_AudioFX;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//
	class UEquipmentComponent* EquipmentComponent = nullptr;

private:
	class AActor* Owner = nullptr;
};
