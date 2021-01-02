// Fill out your copyright notice in the Description page of Project Settings.

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
	class UAnimMontage* DeathAnimation;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// API
	void Roll();
	void Attack01();
	void Attack02();
	void Block01();
	void Block02();
	void SwapWeapon();
	void Hitted();
	void Death();
	EActionType GetActionType() const;

private:
	class ACharacter* Owner = nullptr;
	class UEquipmentComponent* EquipmentComponent = nullptr;
	void SimulatedTick(float DeltaTime);
	void PlayActionAnimation();
	void PlayAnimation(UAnimMontage* ActionAnimation, float StartTime, float InPlayRate = 1.0f);
};
