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
#include "HUD/HUD_Multiplayer.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	ServerState.CurrentHealth = ServerState.DefaultHealth;
	LocalCurrentHealth = ServerState.CurrentHealth;
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
		
		if (GetPlayerHUD() != nullptr)
		{
			GetPlayerHUD()->UpdateStaminaState(GetCurrentStamina()/GetDefaultStamina());
		}
	}
	else
	{
		BaseSprintMultiplier -= RampThisFrame;
		// change stamina bar percentage
		ChangeCurrentStaminaTo(0.1f);
		if (GetPlayerHUD() != nullptr)
		{
			GetPlayerHUD()->UpdateStaminaState(GetCurrentStamina() / GetDefaultStamina());
		}
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

void UHealthComponent::OnRep_StateChanged()
{
	// Update health state
	if (GetPlayerHUD())
	{
		if (LocalCurrentHealth != ServerState.CurrentHealth)
		{
			GetPlayerHUD()->UpdateHealthState(GetCurrentHealth() / GetDefaultHealth());
			LocalCurrentHealth = ServerState.CurrentHealth;
		}

	}
}

void UHealthComponent::RespawnPlayer()
{
	// Update HUD with 100% state
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->UpdateHealthState(GetDefaultHealth());
		GetPlayerHUD()->UpdateStaminaState(GetDefaultStamina());
	}
	if (!Owner->HasAuthority())
	{
		return;
	}
	if (Owner)
	{
		// Get random start position
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);
		auto RandomStartPlaceIndex = FMath::RandRange(0, FoundActors.Num() - 1);
		auto PlayerStart = Cast<APlayerStart>(FoundActors[RandomStartPlaceIndex]);
		// Unpossess an old pawn
		auto PlayerController = Owner->GetController();
		PlayerController->UnPossess();
		// Spawn new pawn
		auto PlayerClass = Owner->GetClass();
		auto NewCharacter = GetWorld()->SpawnActor<AActor>(PlayerClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
		//GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(PlayerController, PlayerStart);
		
		// Possess new pawn if exists
		if (NewCharacter)
		{
			auto CharacterToPossess = Cast<ACharacter>(NewCharacter);
			if (CharacterToPossess)
			{
				PlayerController->Possess(CharacterToPossess);
				Owner->Destroy();
			}
		}
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

void UHealthComponent::SetVulnerability(bool IsVulnerable)
{
	if (Owner->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Vulnerability set on server side"))
		ServerState.bIsVulnerable = IsVulnerable;
	}
}

void UHealthComponent::TakeDamage(AActor* DamagedActor,
									float Damage,
									const UDamageType* DamageType,
									AController* InstigatedBy,
									AActor* DamageCauser)
{
	// Character may be invulnerable e.g. while rolling or use protection magic
	if (ServerState.bIsVulnerable || bIsDead)
	{
		return;
	}
	ServerState.CurrentHealth = FMath::Clamp(ServerState.CurrentHealth - Damage, 0.0f, ServerState.DefaultHealth);
	//Update HUD health status
	if (GetPlayerHUD())
	{
		GetPlayerHUD()->UpdateHealthState(GetCurrentHealth()/GetDefaultHealth());
	}
	if (GetCurrentHealth() <= 0)
	{
		bIsDead = true;
		Death();
	}
	else
	{
		CombatComponent->Hitted();
	}
}

void UHealthComponent::Death()
{
	if (CombatComponent)
	{
		CombatComponent->Death();
	}
}

AHUD_Multiplayer* UHealthComponent::GetPlayerHUD()
{
	if (Owner)
	{
		// We only want locally controlled and controlled by player, not AI
		if (!Owner->IsLocallyControlled())
		{
			return nullptr;
		}

		auto PlayerController = Cast<APlayerController>(Owner->GetController());
		if (!PlayerController)
		{
			return nullptr;
		}
		auto HUD = PlayerController->GetHUD();
		return Cast<AHUD_Multiplayer>(HUD);
	}
	return nullptr;
}
