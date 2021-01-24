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

#include "CustomPlayerState.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	HealthServerState.CurrentHealth = HealthServerState.DefaultHealth;
	LocalCurrentHealth = HealthServerState.CurrentHealth;
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
	// Update HUD with 100% state
	if (GetPlayerHUD() != nullptr)
	{
		GetPlayerHUD()->UpdateHealthState(GetDefaultHealth());
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
	if (bIsDead)
	{
		return;
	}
	// Character may be invulnerable e.g. while rolling or use protection magic
	if (HealthServerState.bIsInvulnerable && GetCurrentHealth() > 0.0f)
	{
		return;
	}
	// Check if the character is blocking
	if (Owner && CombatComponent && InstigatedBy)
	{
		float AngleBetweenActors = FMath::RadiansToDegrees(acosf(FVector::DotProduct(Owner->GetActorForwardVector(), 
													InstigatedBy->GetPawn()->GetActorForwardVector())));
		UE_LOG(LogTemp, Warning, TEXT("HEALTHCOMP: angle = %f"), AngleBetweenActors);
		if (AngleBetweenActors > 90 && CombatComponent->bIsBlocking())
		{// TODO Set appropriate stamina cost
			CombatComponent->Blocked();
			return;
		}
		else
		{
			CombatComponent->Hitted();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO instigator"));
	}
	
	// Update HP if character was damaged
	HealthServerState.CurrentHealth = FMath::Clamp(HealthServerState.CurrentHealth - Damage, 0.0f, HealthServerState.DefaultHealth);
	//Update HUD health status
	if (GetPlayerHUD())
	{
		GetPlayerHUD()->UpdateHealthState(GetCurrentHealth() / GetDefaultHealth());
	}
	// Check if the character has 0 health points
	if (GetCurrentHealth() <= 0.0f)
	{
		bIsDead = true;
		SetScores(InstigatedBy);
		Death();
		return;
	}
}

void UHealthComponent::SetScores(AController* InstigatedBy)
{
	auto KilledBy = InstigatedBy->GetPlayerState<APlayerState>();
	if (KilledBy)
	{
		KilledBy->SetScore(KilledBy->GetScore() + 1);
	}
	auto PlayerController = Cast<APlayerController>(Owner->GetController());
	if (PlayerController)
	{
		auto PlayerState = Cast<ACustomPlayerState>(PlayerController->GetPlayerState<APlayerState>());
		if (PlayerState)
		{
			PlayerState->SetDeaths(PlayerState->GetDeaths() + 1);
		}
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
