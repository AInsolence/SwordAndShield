// Copyright © 2021 Insolence Assets. All Rights Reserved.

#include "Components/HealthComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "HUD/HUD_Multiplayer.h"
#include "Items/Weapon.h"
#include "../Multiplayer_01Character.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	HealthServerState.CurrentHealth = DefaultHealth;
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

	// Update health points for client (begin play + respawn)
	Client_UpdateHealthBar();
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UHealthComponent, HealthServerState, COND_None);
}

void UHealthComponent::OnRep_StateChanged()
{
	// Update health state
	if (GetPlayerHUD())
	{
		if (LocalCurrentHealth != HealthServerState.CurrentHealth)
		{
			GetPlayerHUD()->UpdateHealthState(GetCurrentHealth() / GetDefaultHealth());
			LocalCurrentHealth = HealthServerState.CurrentHealth;
		}
	}
}

void UHealthComponent::RespawnPlayer()
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
	auto NewCharacter = GetWorld()->SpawnActor<AActor>(PlayerClass, PlayerStart->GetActorLocation(), FRotator::ZeroRotator);
	//GetWorld()->GetAuthGameMode()->RestartPlayerAtPlayerStart(PlayerController, PlayerStart);

	// Possess new pawn if exists
	if (NewCharacter)
	{
		auto CharacterToPossess = Cast<AMultiplayer_01Character>(NewCharacter);
		if (CharacterToPossess)
		{
			// Possess after respawn
			PlayerController->Possess(CharacterToPossess);
			// Update HUD after respawn for listen-client
			if (CharacterToPossess->HealthComponent && CharacterToPossess->HasAuthority())
			{
				CharacterToPossess->HealthComponent->Client_UpdateHealthBar();
			}
			// Destroy old pawn
			Owner->Destroy();
		}
	}
}

void UHealthComponent::Client_UpdateHealthBar_Implementation()
{
	if (GetPlayerHUD())
	{
		GetPlayerHUD()->UpdateHealthState(1);
		GetPlayerHUD()->UpdateManaState(1);
	}
}

void UHealthComponent::Server_SetDeadState_Implementation(bool isDead)
{
	HealthServerState.bIsDead = isDead;
}

void UHealthComponent::SetVulnerability(bool IsInvulnerable)
{
	if (Owner->HasAuthority())
	{
		HealthServerState.bIsInvulnerable = IsInvulnerable;
	}
}

void UHealthComponent::TakeDamage(AActor* DamagedActor,
									float Damage,
									const UDamageType* DamageType,
									AController* InstigatedBy,
									AActor* DamageCauser)
{
	// Check if the character already dead
	if (HealthServerState.bIsDead && GetCurrentHealth() <= 0.0f)
	{
		return;
	}
	// Character may be invulnerable e.g. while rolling or use protection magic
	if (HealthServerState.bIsInvulnerable && GetCurrentHealth() > 0.0f)
	{
		return;
	}
	//
	if (Owner && CombatComponent && InstigatedBy)
	{
		// Check if the character is blocking
		float AngleBetweenActors = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Owner->GetActorForwardVector(), 
													InstigatedBy->GetPawn()->GetActorForwardVector())));
		if (AngleBetweenActors > 90 && CombatComponent->bIsBlocking() && !InstigatedBy->ActorHasTag("MagicDamage"))
		{// TODO Set appropriate stamina cost
			CombatComponent->Blocked();
			OnBlocked.Broadcast();
			OnBlockedStaminaCost.Broadcast(20.f);
			return;
		}
		else
		{
			// Update HP if character was damaged
			HealthServerState.CurrentHealth = FMath::Clamp(HealthServerState.CurrentHealth - Damage, 0.0f, DefaultHealth);
			auto HitSound = Cast<AMultiplayer_01Character>(InstigatedBy->GetPawn())->GetActiveWeaponHitSound();
			OnHitted.Broadcast(HitSound);
			//Update HUD health status
			if (GetPlayerHUD())
			{
				GetPlayerHUD()->UpdateHealthState(GetCurrentHealth() / GetDefaultHealth());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO instigator"));
	}
	
	// Check if the character has 0 health points
	if (GetCurrentHealth() <= 0.0f)
	{
		DeathEvent.Broadcast(InstigatedBy);
		Server_SetDeadState(true);
		if (!Owner->HasAuthority())
		{
			return;
		}
		// Set timer to respawn after a death animation
		FTimerHandle    handle;
		float delayTime = 3.5f;
		GetWorld()->GetTimerManager().SetTimer(handle, 
											   [this]()
											   {
												   RespawnPlayer();
											   },
											   delayTime, 
											   false);
		return;
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
