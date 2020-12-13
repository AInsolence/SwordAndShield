// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/EquipmentComponent.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	// Set owner reference
	Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		auto EquipmentCompObject = GetOwner()->GetDefaultSubobjectByName("EquipmentComponent");
		if (EquipmentCompObject)
		{
			EquipmentComponent = Cast<UEquipmentComponent>(EquipmentCompObject);
		}
	}
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ServerActionState.ActionType == EActionType::Roll)
	{
		Owner->AddMovementInput(Owner->GetActorForwardVector(), 1.0f, true);
	}
	//
	if (GetOwnerRole() == ROLE_Authority)
	{
		DrawDebugString(GetWorld(),
			FVector(0, 0, 150),
			ServerActionState.ActionState,
			GetOwner(),
			FColor::White,
			0.1f);
	}
	// 
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		DrawDebugString(GetWorld(),
			FVector(0, 0, 150),
			ServerActionState.ActionState,
			GetOwner(),
			FColor::White,
			0.1f);
	}
	//
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		SimulatedTick(DeltaTime);
	}
}

void UCombatComponent::SimulatedTick(float DeltaTime)
{
	DrawDebugString(GetWorld(),
		FVector(0, 0, 150),
		ServerActionState.ActionState,
		GetOwner(),
		FColor::White,
		0.1f);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCombatComponent, ServerActionState, COND_None);
}

void UCombatComponent::SetServerActionState(const FServerActionState& _ServerActionState)
{
	ServerActionState = _ServerActionState;
}

void UCombatComponent::SwapWeaponEquip()
{
	if (EquipmentComponent)
	{	
		// Call equipment swap logic in the middle of animation
		EquipmentComponent->SwapWeapon();
	}
}

const FServerActionState UCombatComponent::CreateServerActionState(bool _bCanAct, bool _bAnimStart, FString _ActionState, EActionType _ActionType)
{
	FServerActionState State;
	State.bCanAct = _bCanAct;
	State.bAnimStarted = _bAnimStart;
	State.ActionState = _ActionState;
	State.ActionType = _ActionType;
	return State;
}

void UCombatComponent::OnRep_ServerActionState()
{	
	PlayActionAnimation();
	return;
}

void UCombatComponent::PlayActionAnimation()
{
	if (!EquipmentComponent)
	{
		return;
	}
	
	switch (ServerActionState.ActionType)
	{
	case EActionType::None:
		return;
		break;
	case EActionType::Roll:
		PlayAnimation(RollAnimation, 0.5f);
		break;
	case EActionType::Interact:
		break;
	case EActionType::Action_01:
		break;
	case EActionType::Action_02:
		break;
	case EActionType::RightHandAction_01:
		if (EquipmentComponent->RightHandItem)
		{
			PlayAnimation(EquipmentComponent->RightHandItem->UseAnimation_01, 0.0f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO RIGHT HAND ITEM"))
		}
		break;
	case EActionType::RightHandAction_02:
		if (EquipmentComponent->RightHandItem)
		{
			PlayAnimation(EquipmentComponent->RightHandItem->UseAnimation_02, 0.0f);
		}
		break;
	case EActionType::LeftHandAction_01:
		if (EquipmentComponent->LeftHandItem)
		{
			PlayAnimation(EquipmentComponent->LeftHandItem->UseAnimation_01, 0.0f);
		}
		break;
	case EActionType::LeftHandAction_02:
		if (EquipmentComponent->LeftHandItem)
		{
			PlayAnimation(EquipmentComponent->LeftHandItem->UseAnimation_02, 0.0f);
		}
		break;
	case EActionType::SwapWeapon:
		PlayAnimation(SwapWeaponAnimation, 0.0f);
		break;
	case EActionType::Hitted:
		PlayAnimation(HittedAnimation, 0.0f);
		break;
	case EActionType::Death:
		PlayAnimation(DeathAnimation, 0.0f);
		break;
	default:
		break;
	}
}

void UCombatComponent::PlayAnimation(UAnimMontage* ActionAnimation, float StartTime)
{
	if (!Owner || !ActionAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Play anim no animation"))
		auto DeclineStateChange = CreateServerActionState(true, false, "NONE", EActionType::None);
		SetServerActionState(DeclineStateChange);
		return;
	}

	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		ServerActionState.bAnimStarted = true;
		AnimInstance->Montage_Play(ActionAnimation, 1.0f, EMontagePlayReturnType::MontageLength, StartTime);
	}
}

void UCombatComponent::Server_Act_Implementation(EActionType _ActionType)
{
	if (!Owner)
	{
		return;
	}
	if (ServerActionState.ActionType == EActionType::None && ServerActionState.bCanAct)
	{
		auto PrepareToChangeState = CreateServerActionState(false, true, "Action", _ActionType);
		SetServerActionState(PrepareToChangeState);
		// Play animation on the server for this client
		PlayActionAnimation();
	}
	// Hitted and Death animations break other animations
	if (ServerActionState.ActionType == EActionType::Hitted || ServerActionState.ActionType == EActionType::Death)
	{
		auto PrepareToChangeState = CreateServerActionState(false, true, "Action", _ActionType);
		SetServerActionState(PrepareToChangeState);
		// Play animation on the server for this client
		PlayActionAnimation();
	}
}

bool UCombatComponent::Server_Act_Validate(EActionType _ActionType)
{
	return true; // TODO anti-cheat implementation
}

void UCombatComponent::Roll()
{
	Server_Act(EActionType::Roll);
}

void UCombatComponent::Attack01()
{
	Server_Act(EActionType::RightHandAction_01);
}

void UCombatComponent::Attack02()
{
	Server_Act(EActionType::RightHandAction_02);
}

void UCombatComponent::Block01()
{
	Server_Act(EActionType::LeftHandAction_01);
}

void UCombatComponent::Block02()
{
	Server_Act(EActionType::LeftHandAction_02);
}

void UCombatComponent::SwapWeapon()
{
	Server_Act(EActionType::SwapWeapon);
}

void UCombatComponent::Hitted()
{
	Server_Act(EActionType::Hitted);
}

void UCombatComponent::Death()
{
	Server_Act(EActionType::Death);
}
