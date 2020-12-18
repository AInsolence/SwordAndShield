// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "HealthComponent.generated.h"

USTRUCT(BlueprintType)
struct FServerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultStamina = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentStamina;
	// Sprint variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float BaseWalkingSpeed = 600.f;
};

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

	// Sprint helper variables
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float BaseSprintMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float MaxSprintMultiplier = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float TimeToMaxSprintSpeed = 2.0f;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "ServerState")
	FServerState ServerState;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "ServerState")
	void Server_ChangeState(bool IsSprinting);
	void Server_ChangeState_Implementation(bool IsSprinting);
	bool Server_ChangeState_Validate(bool IsSprinting);

	UFUNCTION(BlueprintCallable, Category = "HealthProperty")
	void SetVulnerability(bool IsVulnerable);

	UFUNCTION(BlueprintCallable, Category = "HealthProperty")
	void TakeDamage(AActor* DamagedActor,
					float Damage,
					const class UDamageType* DamageType,
					class AController* InstigatedBy,
					AActor* DamageCauser);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE float GetDefaultHealth() const { return ServerState.DefaultHealth; };
	FORCEINLINE float GetCurrentHealth() const { return ServerState.CurrentHealth; };
	FORCEINLINE float GetDefaultStamina() const { return ServerState.DefaultStamina; };
	FORCEINLINE float GetCurrentStamina() const { return ServerState.CurrentStamina; };

	UFUNCTION(BlueprintCallable)
	void ChangeCurrentStaminaTo(float StaminaCost);
	UFUNCTION(BlueprintCallable)
	void SetIsSprinting(bool IsSprinting);
	UFUNCTION(BlueprintCallable)
	void RespawnPlayer();

private:
	class ACharacter* Owner = nullptr;
	class UCombatComponent* CombatComponent = nullptr;
	bool bIsVulnerable = false;
	void Death();
};
