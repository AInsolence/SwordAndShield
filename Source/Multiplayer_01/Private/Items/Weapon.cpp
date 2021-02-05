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
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	DamageCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageCollisionBox"));
	PickUpCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpCollisionBox"));

	if (WeaponMesh && PickUpCollisionBox && DamageCollisionBox)
	{
		PickUpCollisionBox->SetupAttachment(WeaponMesh);
		DamageCollisionBox->SetupAttachment(WeaponMesh);
		DamageCollisionBox->SetIsReplicated(true);
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
	DamageCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
	DamageCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnOverlapEnd);

	bAlwaysRelevant = true;
}

void AWeapon::OnRep_Activated()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Activated is called"))
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
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Use is called from weapon c++ class"))
		bActivated = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Use has no authority"))
	}
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
	GetOverlappedEnemy(OtherActor);
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
						   AActor* OtherActor, 
						   UPrimitiveComponent* OtherComp,
						   int32 OtherBodyIndex)
{
	GetOverlappedEnemy(OtherActor);
}

void AWeapon::GetOverlappedEnemy(AActor* OtherActor)
{
	// Check if overlap event calls on the server 
	if (!HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Not AUTHORITY"))
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
		else if (OtherActor->GetClass()->IsChildOf(ACharacter::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Broadcast begin/end with %s"), *OtherActor->GetName());
			OnWeaponOverlap.Broadcast(OtherActor);
		}
	}
}
