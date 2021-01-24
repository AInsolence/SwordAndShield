// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This is a main class of a player's character. Contains all components and API to provide
	player with controlls.

***/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Multiplayer_01Character.generated.h"

UCLASS(config=Game)
class AMultiplayer_01Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	AMultiplayer_01Character();

	virtual void Tick(float DeltaTime);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Combat component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatComponent;

	/** Equipment component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UEquipmentComponent* EquipmentComponent;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

	/** Health component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UStaminaComponent* StaminaComponent;

	/** Interaction component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UInteractionComponent* InteractionComponent;

	/** Health bar component which would be shown to enemies */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarComponent;

	/** Statistics component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UStatisticsComponent* StatisticsComponent;

	// Change player name console command
	UFUNCTION(Exec)
	void name(FString Name);
	UFUNCTION(Server, Reliable, WithValidation, Category = "ConsoleCommands")
	void Server_SetPlayerName(const FString& Name);
	void Server_SetPlayerName_Implementation(const FString& Name);
	bool Server_SetPlayerName_Validate(const FString& Name);

protected:

	virtual void BeginPlay();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Roll();
	void Sprint();
	float SprintRollPressedTime = 0.0f;

	void Attack01();
	void Attack02();
	void Block01();
	void UnBlock01();
	void Block02();
	void SwapWeapon();
	void Interact();

	void ShowStatistic();
	void HideStatistic();

	bool bIsActionPossible(float StaminaCost);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
