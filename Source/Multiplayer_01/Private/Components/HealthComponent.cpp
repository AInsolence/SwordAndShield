// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
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
		// Add Take damage to the delegate
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);
		// Set combat component to call animations
		auto CombatCompObject = GetOwner()->GetDefaultSubobjectByName("CombatComponent");
		if (CombatCompObject)
		{
			CombatComponent = Cast<UCombatComponent>(CombatCompObject);
		}
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

void UHealthComponent::RespawnPlayer()
{
	if (!Owner->HasAuthority())
	{
		return;
	}
	if (Owner)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);
		auto PlayerStart = Cast<APlayerStart>(FoundActors[0]);
		
		auto PlayerController = Owner->GetController();
		PlayerController->UnPossess();

		auto PlayerClass = Owner->GetClass();
		
		UE_LOG(LogTemp, Warning, TEXT("Player start location %s"), *PlayerStart->GetActorLocation().ToString());
		UE_LOG(LogTemp, Warning, TEXT("Player location %s"), *Owner->GetActorLocation().ToString());

		auto NewCharacter = GetWorld()->SpawnActor<AActor>(PlayerClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
		//GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(PlayerController, PlayerStart);
		
		if (NewCharacter)
		{
			auto CharacterToPossess = Cast<ACharacter>(NewCharacter);
			if (CharacterToPossess)
			{
				PlayerController->Possess(CharacterToPossess);
				Owner->Destroy();
			}
		}
		//ServerState.CurrentHealth = 100.f;
	}
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
	ServerState.CurrentHealth = FMath::Clamp(ServerState.CurrentHealth - Damage, 0.0f, ServerState.DefaultHealth);
	CombatComponent->Hitted();
	//Update HUD health status
	/*if (GetPlayerHUD())
	{
		GetPlayerHUD()->UpdateHealthState(HealthComponent->GetCurrentHealth() /
			HealthComponent->GetDefaultHealth());
	}*/
	if (GetCurrentHealth() <= 0)
	{
		Death();
	}
}

void UHealthComponent::Death()
{
	if (CombatComponent)
	{// Play death animation
		CombatComponent->Death();
	}
	// Disable collision capsule if the character is dead
	/*auto Capsule = Owner->FindComponentByClass<UCapsuleComponent>();
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}*/
	// Disable controller
	AController* CurrentController = Owner->GetController();
	if (CurrentController) {
		// stop movement so the death animation plays immediately
		//CurrentController->StopMovement();
		//Owner->GetMesh()->bPauseAnims = true;
		/* AI logic option */
		// un-possess to stop AI
		//CurrentController->UnPossess();
		// destroy the controller, since it's not part of the enemy anymore
		//CurrentController->Destroy();
	}
}
