// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Components/Image.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Components/CombatComponent.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	DamageCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollisionBox"));
	PickUpCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpCollisionBox"));

	if (WeaponMesh && PickUpCollisionBox && DamageCollisionBox)
	{
		PickUpCollisionBox->SetupAttachment(WeaponMesh);
		DamageCollisionBox->SetupAttachment(WeaponMesh);
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (!DamageCollisionBox)
	{
		return;
	}
	// Add OnOverlap to the delegate
	DamageCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlap);
}

void AWeapon::SetOwner(AActor* _Owner)
{
	WeaponOwner = _Owner;
}

AActor* AWeapon::GetOwner()
{
	return WeaponOwner;
}

UClass* AWeapon::PickUp()
{
	auto ItemClass = GetClass();
	UE_LOG(LogTemp, Warning, TEXT("Weapon picked up"))
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon try to SERVER on MAP destroy"))
		Destroy();
	}
	return ItemClass;
}

void AWeapon::Use()
{
	bActivated = true;
}

USkeletalMeshComponent* AWeapon::GetItemMesh()
{
	return WeaponMesh;
}

UImage* AWeapon::GetItemImage()
{
	return nullptr;
}

void AWeapon::OnOverlap(UPrimitiveComponent* OverlappedComponent, 
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult)
{
	// Check is weapon active
	if (!bActivated)
	{
		return;
	}
	// 
	if (WeaponOwner)
	{
		// Check for self-overlapping
		if (WeaponOwner == OtherActor)
		{
			return;
		}
		// Check is overlap character
		else if (OtherActor->GetClass()->IsChildOf(ACharacter::StaticClass()))
		{
			// Create a damage event  
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);
			// Take damage to the overlapped actor
			OtherActor->TakeDamage(Damage,
									DamageEvent,
									GetInstigatorController(),
									this);
			// Deactivate weapon after damage was caused
			bActivated = false;
		}
	}
}
