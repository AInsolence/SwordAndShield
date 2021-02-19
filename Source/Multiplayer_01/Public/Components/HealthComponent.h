// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class is an actor component represents player's health properties.

***/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "HealthComponent.generated.h"

USTRUCT(BlueprintType)
struct FHealthServerState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 99.f;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsInvulnerable = false;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsDead = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnDeath, AController*, InstigatedBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitted, USoundBase*, SoundBase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlocked);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_StateChanged, EditDefaultsOnly, BlueprintReadWrite, Category = "ServerState")
	FHealthServerState HealthServerState;
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void OnRep_StateChanged();

	UFUNCTION(BlueprintCallable, Category = "HealthProperty")
	void SetVulnerability(bool IsInvulnerable);

	UFUNCTION(BlueprintCallable, Category = "HealthProperty")
	void TakeDamage(AActor* DamagedActor,
					float Damage,
					const class UDamageType* DamageType,
					class AController* InstigatedBy,
					AActor* DamageCauser);

public:
	// API
	FORCEINLINE float GetDefaultHealth() const { return HealthServerState.DefaultHealth; };
	float GetCurrentHealth() const { return HealthServerState.CurrentHealth; };

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "HealthProperty")
	void Server_SetDeadState(bool isDead);
	void Server_SetDeadState_Implementation(bool isDead);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "HealthProperty")
	void Client_UpdateHealthBar();
	void Client_UpdateHealthBar_Implementation();

	UFUNCTION(BlueprintCallable, Category = "HealthProperty")
	void RespawnPlayer();
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnDeath DeathEvent;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnHitted OnHitted;
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnBlocked OnBlocked;

private:
	class ACharacter* Owner = nullptr;
	class UCombatComponent* CombatComponent = nullptr;
	float LocalCurrentHealth = 0.0;
	class AHUD_Multiplayer* GetPlayerHUD();
};
