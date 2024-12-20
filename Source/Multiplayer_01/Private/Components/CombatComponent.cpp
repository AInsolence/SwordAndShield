// Copyright � 2021 Insolence Assets. All Rights Reserved.

#include "Components/CombatComponent.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/EquipmentComponent.h"
#include "Components/HealthComponent.h"


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
		//
		auto HealthCompObject = GetOwner()->GetDefaultSubobjectByName("HealthComponent");
		if (HealthCompObject)
		{
			HealthComponent = Cast<UHealthComponent>(HealthCompObject);
			if (HealthComponent)
			{
				HealthComponent->DeathEvent.AddDynamic(this, &UCombatComponent::Death);
			}
		}
	}
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//
	if (ServerActionState.ActionType == EActionType::Roll)
	{
		Owner->AddMovementInput(Owner->GetActorForwardVector(), 1.0f, true);
	}
	//
	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		SimulatedTick(DeltaTime);
	}
}

void UCombatComponent::SimulatedTick(float DeltaTime)
{
	// Simulated actions here
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCombatComponent, ServerActionState, COND_None);
}

void UCombatComponent::Server_Act_Implementation(EActionType _ActionType)
{
	if (ServerActionState.ActionType == EActionType::Death || !Owner)
	{
		return;
	}
	// Hitted and Death animations break other animations
	if (_ActionType == EActionType::Death || _ActionType == EActionType::Hitted)
	{
		auto PrepareToChangeState = CreateServerActionState(false, _ActionType);		
		SetServerActionState(PrepareToChangeState);
		// Play animation on the server for this client
		PlayActionAnimation();
	}
	else if (ServerActionState.ActionType == EActionType::None && ServerActionState.bCanAct)
	{
		auto PrepareToChangeState = CreateServerActionState(false, _ActionType);
		SetServerActionState(PrepareToChangeState);
		// Play animation on the server for this client
		PlayActionAnimation();
	}
}

void UCombatComponent::Server_SetBlocking_Implementation(bool IsBlocking)
{
	ServerActionState.bIsBlocking = IsBlocking;
}

bool UCombatComponent::Server_SetBlocking_Validate(bool IsBlocking)
{
	return true;
}

bool UCombatComponent::bIsBlocking() const
{
	return ServerActionState.bIsBlocking && (ServerActionState.ActionType == EActionType::None);
}

void UCombatComponent::SetServerActionState(const FServerActionState& _ServerActionState)
{
	ServerActionState = _ServerActionState;
}

const FServerActionState UCombatComponent::CreateServerActionState(bool _bCanAct, EActionType _ActionType)
{
	FServerActionState State;
	State.bCanAct = _bCanAct;
	State.ActionType = _ActionType;
	State.bIsBlocking = ServerActionState.bIsBlocking;
	return State;
}

void UCombatComponent::OnRep_ServerActionState()
{
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
		break;
	case EActionType::Roll:
		Multicast_PlayAnimation(RollAnimation, 0.6f);
		break;
	case EActionType::Interact:
		break;
	case EActionType::Action_01:
		break;
	case EActionType::Action_02:
		break;
	case EActionType::RightHandAction_01:
		if (EquipmentComponent->Equipment[0])
		{
			Multicast_PlayAnimation(EquipmentComponent->Equipment[0]->UseAnimation_01, 0.0f,
						  EquipmentComponent->Equipment[0]->SpeedOfAttack01);
		}
		break;
	case EActionType::RightHandAction_02:
		if (EquipmentComponent->Equipment[0])
		{
			Multicast_PlayAnimation(EquipmentComponent->Equipment[0]->UseAnimation_02, 0.0f,
						  EquipmentComponent->Equipment[0]->SpeedOfAttack02);
		}
		break;
	case EActionType::LeftHandAction_01:
		if (EquipmentComponent->Equipment[1])
		{
			Multicast_PlayAnimation(EquipmentComponent->Equipment[1]->UseAnimation_01, 0.0f,
						  EquipmentComponent->Equipment[1]->SpeedOfAttack01);
		}
		break;
	case EActionType::LeftHandAction_02:
		if (EquipmentComponent->Equipment[1])
		{
			Multicast_PlayAnimation(EquipmentComponent->Equipment[1]->UseAnimation_02, 0.0f,
						  EquipmentComponent->Equipment[1]->SpeedOfAttack02);
		}
		break;
	case EActionType::SwapWeapon:
		Multicast_PlayAnimation(SwapWeaponAnimation, 0.0f);
		break;
	case EActionType::Hitted:
		Multicast_PlayAnimation(HittedAnimation, 0.0f);
		break;
	case EActionType::Blocked:
		Multicast_PlayAnimation(BlockedAnimation, 0.0f);
		break;
	case EActionType::Death:
		Multicast_PlayAnimation(DeathAnimation, 0.0f);
		break;
	default:
		break;
	}
}

void UCombatComponent::Multicast_PlayAnimation_Implementation(UAnimMontage* ActionAnimation, float StartTime, float InPlayRate)
{
	if (!Owner || !ActionAnimation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot find animation"))
		auto DeclineStateChange = CreateServerActionState(true, EActionType::None);
		SetServerActionState(DeclineStateChange);
		return;
	}
	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(ActionAnimation, InPlayRate, EMontagePlayReturnType::MontageLength, StartTime);
	}
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

void UCombatComponent::Block01(bool IsBlocking)
{
	Server_SetBlocking(IsBlocking);
}

void UCombatComponent::SpecialAttack()
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

void UCombatComponent::Blocked()
{
	Server_Act(EActionType::Blocked);
}

void UCombatComponent::Death(AController* InstigatedBy)
{
	Server_Act(EActionType::Death);
}

EActionType UCombatComponent::GetActionType() const
{
	return ServerActionState.ActionType;
}

bool UCombatComponent::bCanCharacterAct() const
{
	return ServerActionState.bCanAct;
}
