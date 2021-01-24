// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class is an actor component represents player's stamina and
	player's sprint properties.

***/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "StaminaComponent.generated.h"

USTRUCT(BlueprintType)
struct FStaminaServerState
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultStamina = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentStamina;

	// Sprint variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	bool bIsSprinting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float BaseWalkingSpeed = 500.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Sprint helper variables
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float BaseSprintMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float MaxSprintMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float TimeToMaxSprintSpeed = 2.0f;

	UPROPERTY(ReplicatedUsing = OnRep_StateChanged, EditDefaultsOnly, BlueprintReadWrite, Category = "ServerState")
	FStaminaServerState StaminaServerState;
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void OnRep_StateChanged();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "ServerState")
	void Server_ChangeState(bool IsSprinting);
	void Server_ChangeState_Implementation(bool IsSprinting);
	bool Server_ChangeState_Validate(bool IsSprinting);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "ServerState")
	void Server_SetCurrentStamina(float StaminaCost);
	void Server_SetCurrentStamina_Implementation(float StaminaCost);
	bool Server_SetCurrentStamina_Validate(float StaminaCost);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//API
	FORCEINLINE float GetDefaultStamina() const { return StaminaServerState.DefaultStamina; };
	float GetCurrentStamina() const { return StaminaServerState.CurrentStamina; };
	UFUNCTION(BlueprintCallable, Category = "StaminaProperty")
	void ChangeCurrentStaminaTo(float StaminaCost);
	UFUNCTION(BlueprintCallable, Category = "StaminaProperty")
	void NotEnoughStamina();
	UFUNCTION(BlueprintCallable, Category = "StaminaProperty")
	void SetIsSprinting(bool IsSprinting);

private:
	class ACharacter* Owner = nullptr;
	class AHUD_Multiplayer* GetPlayerHUD();
};
