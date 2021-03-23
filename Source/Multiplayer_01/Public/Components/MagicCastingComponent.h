// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "MagicCastingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagicCast, USoundBase*, SoundBase);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UMagicCastingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMagicCastingComponent();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "MagicCasting")
	void Server_CreateMagicProjectile(TSubclassOf<class AMagicProjectile> Projectile);
	void Server_CreateMagicProjectile_Implementation(TSubclassOf<class AMagicProjectile> Projectile);

	FOnMagicCast OnMagicCast;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MagicCasting")
	class USoundBase* MagicCast_AudioFX;
};
