// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	ServerState.CurrentHealth = ServerState.DefaultHealth;
	ServerState.CurrentStamina = ServerState.DefaultStamina;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!Owner)
	{
		return;
	}
	/**  Sprint logic */
	float RampThisFrame = (DeltaTime / TimeToMaxSprintSpeed) * MaxSprintMultiplier;
	// check if sprint input pressed, is character has enough stamina and is he moves
	if (ServerState.bIsSprinting && !Owner->GetVelocity().IsZero() &&
		GetCurrentStamina() > 0.5f)
	{
		BaseSprintMultiplier += RampThisFrame;
		// change stamina bar percentage
		ChangeCurrentStaminaTo(-0.5f);
		/*
		if (GetPlayerHUD() != nullptr)
		{
			GetPlayerHUD()->UpdateStaminaState(HealthComponent->GetCurrentStamina() /
			HealthComponent->GetDefaultStamina());
		}*/
	}
	else
	{
		BaseSprintMultiplier -= RampThisFrame;
		// change stamina bar percentage
		ChangeCurrentStaminaTo(0.1f);
		/*if (GetPlayerHUD() != nullptr)
		{
			GetPlayerHUD()->UpdateStaminaState(HealthComponent->GetCurrentStamina() /
				HealthComponent->GetDefaultStamina());
		}*/
	}
	BaseSprintMultiplier = FMath::Clamp(BaseSprintMultiplier, 1.0f, MaxSprintMultiplier);
	Owner->GetCharacterMovement()->MaxWalkSpeed = ServerState.BaseWalkingSpeed * BaseSprintMultiplier;
	/**  Sprint logic end */
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set owner reference
	Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		// Set combat component to call animations
		auto CombatCompObject = GetOwner()->GetDefaultSubobjectByName("CombatComponent");
		if (CombatCompObject)
		{
			CombatComponent = Cast<UCombatComponent>(CombatCompObject);
		}
		// set base speed sprint variables
		ServerState.BaseWalkingSpeed = Owner->GetCharacterMovement()->MaxWalkSpeed;
		//
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHealthComponent, ServerState, COND_None);
}

void UHealthComponent::ChangeCurrentStaminaTo(float StaminaCost)
{
	ServerState.CurrentStamina = FMath::Clamp(ServerState.CurrentStamina + StaminaCost, 0.0f, ServerState.DefaultStamina);
}

void UHealthComponent::SetIsSprinting(bool IsSprinting)
{
	Server_ChangeState(IsSprinting);
}

void UHealthComponent::Server_ChangeState_Implementation(bool IsSprinting)
{
	ServerState.bIsSprinting = IsSprinting;
}

bool UHealthComponent::Server_ChangeState_Validate(bool IsSprinting)
{
	return true; // Change to anti-cheat function
}

void UHealthComponent::TakeDamage(AActor* DamagedActor,
									float Damage,
									const UDamageType* DamageType,
									AController* InstigatedBy,
									AActor* DamageCauser)
{
	if (!CombatComponent)
	{
		return;
	}
	ServerState.CurrentHealth = FMath::Clamp(ServerState.CurrentHealth - Damage, 0.0f, ServerState.DefaultHealth);
	CombatComponent->Hitted();
	//Update HUD health status
	/*if (GetPlayerHUD())
	{
		GetPlayerHUD()->UpdateHealthState(HealthComponent->GetCurrentHealth() /
			HealthComponent->GetDefaultHealth());
	}*/

	//*** DEATH ***//
	if (GetCurrentHealth() <= 0)
	{
		CombatComponent->Death();
		// Disable collision capsule if the character is dead
		auto Capsule = DamagedActor->FindComponentByClass<UCapsuleComponent>();
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		// Disable controller
		AController* CurrentController = Owner->GetController();
		if (CurrentController) {
			// stop movement so the death animation plays immediately
			CurrentController->StopMovement();
			//Owner->GetMesh()->bPauseAnims = true;
			/* AI logic option */
			// un-possess to stop AI
			//CurrentController->UnPossess();
			// destroy the controller, since it's not part of the enemy anymore
			//CurrentController->Destroy();
		}
	}
	// *** DEATH END *** //
}