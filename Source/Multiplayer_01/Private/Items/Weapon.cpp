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
	PrimaryActorTick.bCanEverTick = true;

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
	// set up a notification for when this component hits something blocking
	DamageCollisionBox->OnComponentHit.AddDynamic(this, &AWeapon::OnHit);
	DamageCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlap);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::SetOwner(AActor* _Owner)
{
	WeaponOwner = _Owner;
}

AActor* AWeapon::GetOwner()
{
	return WeaponOwner;
}

void AWeapon::PickUp()
{
	UE_LOG(LogTemp, Warning, TEXT("Interactable item picked up"))
	Destroy();
}

void AWeapon::Use()
{
	bActivated = true;
	UE_LOG(LogTemp, Warning, TEXT("Interactable item used"))
}

USkeletalMeshComponent* AWeapon::GetItemMesh()
{
	return WeaponMesh;
}

UImage* AWeapon::GetItemImage()
{
	return nullptr;
}

void AWeapon::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
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
		UE_LOG(LogTemp, Warning, TEXT("Right hand weapon is not activated"))
		return;
	}
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
			UE_LOG(LogTemp, Warning, TEXT("OVERLAP EVENT CALL WITH : %s"), *OtherActor->GetClass()->GetName());
			float DamageAmount = 30.f;
			// Create a damage event  
			TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent(ValidDamageTypeClass);
			if (GetWorld())
			{
				if (GetWorld()->GetFirstPlayerController())
				{
					OtherActor->TakeDamage(DamageAmount,
											DamageEvent,
											GetWorld()->GetFirstPlayerController(),
											this);
					// Take damage only once
					bActivated = false;
				}
			}
		}
	}
}
