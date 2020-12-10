// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon.h"
#include "Components/Image.h"
#include "Components/BoxComponent.h"

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
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::PickUp()
{
	UE_LOG(LogTemp, Warning, TEXT("Interactable item picked up"))
	Destroy();
}

void AWeapon::Use()
{
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