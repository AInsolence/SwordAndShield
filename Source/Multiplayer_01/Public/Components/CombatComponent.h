// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class is an actor component which manage a player's combat state and 
	communicate with other components.

***/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "CombatComponent.generated.h"

UENUM(BlueprintType)
enum class EActionType : uint8
{
	None,
	Roll,
	Interact,
	Action_01,
	Action_02,
	RightHandAction_01,
	RightHandAction_02,
	LeftHandAction_01,
	LeftHandAction_02,
	SwapWeapon,
	Hitted,
	Blocked,
	Death
};

USTRUCT(BlueprintType)
struct FServerActionState
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category = "ServerActionState")
	bool bCanAct = true;
	UPROPERTY(VisibleAnywhere, Category = "ServerActionState")
	bool bAnimStarted = false;
	UPROPERTY(EditAnywhere, Category = "ServerActionState")
	FString ActionState = "NONE";
	UPROPERTY(EditAnywhere, Category = "ServerActionState")
	EActionType ActionType = EActionType::None;
	UPROPERTY(EditAnywhere, Category = "ServerActionState")
	bool bIsBlocking = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	// Sets default values for this component's properties
	UCombatComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//
	UPROPERTY(ReplicatedUsing = OnRep_ServerActionState, VisibleAnywhere, Category = "ServerActionState")
	FServerActionState ServerActionState;
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void OnRep_ServerActionState();
	//
	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_Act(EActionType _ActionType);
	void Server_Act_Implementation(EActionType _ActionType);
	bool Server_Act_Validate(EActionType _ActionType);
	//
	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_SetBlocking(bool IsBlocking);
	void Server_SetBlocking_Implementation(bool IsBlocking);
	bool Server_SetBlocking_Validate(bool IsBlocking);
	//
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void SetServerActionState(const FServerActionState& _ServerActionState);
	UFUNCTION(BlueprintCallable, Category = "Replication")
	const FServerActionState CreateServerActionState(bool _bCanAct,
													 bool _bAnimStart,
													 FString _ActionState,
													 EActionType _ActionType);

	/** AnimMontages to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* RollAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* SwapWeaponAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* HittedAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* BlockedAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* DeathAnimation;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// API
	void Roll();
	void Attack01();
	void Attack02();
	void Block01(bool IsBlocking);
	void Block02();
	void SwapWeapon();
	void Hitted();
	void Blocked();
	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	void Death(AController* InstigatedBy);
	//
	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	bool bIsBlocking() const;
	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	EActionType GetActionType() const;
	UFUNCTION(BlueprintCallable, Category = "CombatComponent")
	bool bCanCharacterAct() const;

private:
	class ACharacter* Owner = nullptr;
	class UEquipmentComponent* EquipmentComponent = nullptr;
	class UHealthComponent* HealthComponent = nullptr;
	void SimulatedTick(float DeltaTime);
	void PlayActionAnimation();
	void PlayAnimation(UAnimMontage* ActionAnimation, float StartTime, float InPlayRate = 1.0f);
};
