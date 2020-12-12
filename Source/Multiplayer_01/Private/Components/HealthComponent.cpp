// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	CurrentHealth = DefaultHealth;
	CurrentStamina = DefaultStamina;
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
	if (bIsSprinting && !Owner->GetVelocity().IsZero() &&
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
	Owner->GetCharacterMovement()->MaxWalkSpeed = BaseWalkingSpeed * BaseSprintMultiplier;
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
		// set base speed sprint variables
		BaseWalkingSpeed = Owner->GetCharacterMovement()->MaxWalkSpeed;
		//
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHealthComponent, bIsSprinting, COND_None);
}

void UHealthComponent::ChangeCurrentStaminaTo(float StaminaCost)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaCost, 0.0f, DefaultStamina);
}

void UHealthComponent::TakeDamage(AActor* DamagedActor,
									float Damage,
									const UDamageType* DamageType,
									AController* InstigatedBy,
									AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, DefaultHealth);
}
