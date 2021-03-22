// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Components/Image.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Components/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Multiplayer_01/Multiplayer_01Character.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 2;
	bNetUseOwnerRelevancy = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	DamageCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollisionBox"));
	PickUpCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpCollisionBox"));

	if (WeaponMesh && PickUpCollisionBox && DamageCollisionBox)
	{
		PickUpCollisionBox->SetupAttachment(WeaponMesh);
		DamageCollisionBox->SetupAttachment(WeaponMesh);
	}
	// Clear damaged actors list
	ServerAttackState.DamagedActors.Empty();
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!DamageCollisionBox)
	{
		return;
	}

	if (HasAuthority())
	{
		DamageCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
		DamageCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnOverlapEnd);
	}
	if (Cast<APawn>(GetOwner()))
	{
		if (Cast<APawn>(GetOwner())->IsLocallyControlled())
		{
			// Add OnOverlap to the delegate
			DamageCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
			DamageCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnOverlapEnd);
		}
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeapon, ServerAttackState, COND_None);
}

void AWeapon::SetOwner(AActor* _Owner)
{
	AActor::SetOwner(_Owner);
}

AActor* AWeapon::GetOwner()
{
	return AActor::GetOwner();
}

UClass* AWeapon::PickUp()
{
	auto ItemClass = GetClass();

	if (HasAuthority())
	{
		Destroy();
	}
	return ItemClass;
}

void AWeapon::Use()
{
}

USkeletalMeshComponent* AWeapon::GetItemMesh()
{
	return WeaponMesh;
}

UImage* AWeapon::GetItemImage()
{
	return nullptr;
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
										    AActor* OtherActor,
										    UPrimitiveComponent* OtherComp,
						  				    int32 OtherBodyIndex,
										    bool bFromSweep,
										    const FHitResult& SweepResult)
{
	if (!GetOwner() || !Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		return;
	}
	GetOverlappedEnemy(OtherActor);
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
										  AActor* OtherActor, 
										  UPrimitiveComponent* OtherComp,
										  int32 OtherBodyIndex)
{
	if (!GetOwner() || !Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		return;
	}
	GetOverlappedEnemy(OtherActor);
}

void AWeapon::GetOverlappedEnemy(AActor* OtherActor)
{
	if (!OtherActor)
	{
		return;
	}
	// 
	if (GetOwner())
	{
		// Check for self-overlapping
		if (GetOwner() == OtherActor)
		{
			return;
		}
		// Check is overlap character
		if (OtherActor->GetClass()->IsChildOf(ACharacter::StaticClass()))
		{
			OnWeaponOverlap(OtherActor);
		}
	}
}

void AWeapon::Server_AddDamagedActors_Implementation(const FString& ActorName)
{
	ServerAttackState.DamagedActors.Add(ActorName);
}

void AWeapon::Server_ClearDamagedActors_Implementation()
{
	ServerAttackState.DamagedActors.Empty();
}

void AWeapon::Server_SetAttack_Implementation(bool IsAttack)
{
	ServerAttackState.bIsAttack = IsAttack;
	Server_ClearDamagedActors();
}

void AWeapon::OnWeaponOverlap(AActor* OverlappedActor)
{
	if (ServerAttackState.bIsAttack)
	{
		TakeDamageToOverlappedActor(OverlappedActor);
	}
}

void AWeapon::TakeDamageToOverlappedActor_Implementation(AActor* OverlappedActor)
{
	if (!ServerAttackState.DamagedActors.Contains(OverlappedActor->GetName()))
	{
		//// Create a damage event 
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		auto CurrentDamage = GetCurrentAttackDamage();
		// Take damage to the overlapped actor
		OverlappedActor->TakeDamage(CurrentDamage,
									DamageEvent,
									GetOwner()->GetInstigatorController(),
									GetOwner());
		Server_AddDamagedActors(OverlappedActor->GetName());
	}
}

EActionType AWeapon::GetOwnerActionType()
{
	auto OwnerCharacter = Cast<AMultiplayer_01Character>(GetOwner());
	if (OwnerCharacter)
	{
		if (OwnerCharacter->CombatComponent)
		{
			auto CurrentAction = OwnerCharacter->CombatComponent->GetActionType();
			return CurrentAction;
		}
		return EActionType::None;
	}
	return EActionType::None;
}

float AWeapon::GetAttackStaminaCost()
{
	if (GetOwnerActionType() == EActionType::RightHandAction_02)
	{
		return StaminaCost_Attack02;
	}
	else
	{
		return StaminaCost_Attack01;
	}
}

float AWeapon::GetCurrentAttackDamage()
{
	if (GetOwnerActionType() == EActionType::RightHandAction_02)
	{
		return Damage_Attack02;
	}
	else
	{
		return Damage_Attack01;
	}
}
