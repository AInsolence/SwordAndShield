// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "ManaComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UManaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UManaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Sprint helper variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultMana = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_StateChanged, EditDefaultsOnly, BlueprintReadWrite, Category = "ServerState")
	float CurrentMana = 99.f;;
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void OnRep_StateChanged();

public:
	//API
	FORCEINLINE float GetDefaultStamina() const { return DefaultMana; };
	float GetCurrentMana() const { return CurrentMana; };
	UFUNCTION(BlueprintCallable, Category = "ManaProperty")
	void ChangeCurrentManaTo(float ManaCost);
	UFUNCTION(BlueprintCallable, Category = "ManaProperty")
	void NotEnoughMana();

private:
	class ACharacter* Owner = nullptr;
	class AHUD_Multiplayer* GetPlayerHUD();
};
