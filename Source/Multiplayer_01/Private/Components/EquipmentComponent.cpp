// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InteractableItemInterface.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Items/ItemSpawner.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	World = GetWorld();

	Equipment.Init(nullptr, 4);

	//TODO Change to appropriate after test
	if (BaseTestWeapon && BaseTestShield)
	{
		Server_EquipItem(EItemSlot::RightHandItem, BaseTestWeapon);
		Server_EquipItem(EItemSlot::LeftHandItem, BaseTestShield);
		if (BeltTestWeapon)
		{
			Server_EquipItem(EItemSlot::BeltPlaceItem, BeltTestWeapon);
		}
		if (BackTestWeapon)
		{
			Server_EquipItem(EItemSlot::BackPlaceItem, BackTestWeapon);
		}
	}
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UEquipmentComponent, Equipment, COND_None);
}

void UEquipmentComponent::Server_EquipItem_Implementation(EItemSlot ItemSlot, TSubclassOf<class AWeapon> SlotWeapon)
{
	if (!SlotWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT slot weapon"))
		return;
	}
	// Create new weapon in an appropriate socket
	switch (ItemSlot)
	{
		case EItemSlot::RightHandItem:
			if (Equipment[0])
			{
				Equipment[0]->Destroy();
				Equipment[0] = nullptr;
			}
			Equipment[0] = CreateWeaponOnSocket(SlotWeapon, "RightHandWeaponSocket");
			break;
		case EItemSlot::LeftHandItem:
			if (Equipment[1])
			{
				Equipment[1]->Destroy();
				Equipment[1] = nullptr;
			}
			Equipment[1] = CreateWeaponOnSocket(SlotWeapon, "LeftHandWeaponSocket");
			break;
		case EItemSlot::BeltPlaceItem:
			if (Equipment[2])
			{
				Equipment[2]->Destroy();
				Equipment[2] = nullptr;
			}
			Equipment[2] = CreateWeaponOnSocket(SlotWeapon, "BeltWeaponSocket");
			break;
		case EItemSlot::BackPlaceItem:
			if (Equipment[3])
			{
				Equipment[3]->Destroy();
				Equipment[3] = nullptr;
			}
			Equipment[3] = CreateWeaponOnSocket(SlotWeapon, "BackWeaponSocket");
			break;
		default:
			break;
	}
}

bool UEquipmentComponent::Server_EquipItem_Validate(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon)
{
	return true;
}

void UEquipmentComponent::Server_SwapWeapon_Implementation()
{
	if (Equipment[0] == nullptr || 
		Equipment[2] == nullptr)
	{
		return;
	}
	auto TempWeapon = Equipment[0]->GetClass();
	//
	Equipment[0]->Destroy();
	Equipment[0] = CreateWeaponOnSocket(Equipment[2]->GetClass(), "RightHandWeaponSocket");
	//
	Equipment[2]->Destroy();
	Equipment[2] = CreateWeaponOnSocket(TempWeapon, "BeltWeaponSocket");
}

bool UEquipmentComponent::Server_SwapWeapon_Validate()
{
	return true;
}

void UEquipmentComponent::SwapWeapon()
{
	Server_SwapWeapon();
}

void UEquipmentComponent::DropWeapon(FVector SpawnLocation)
{
	SpawnItemInAWorld(Equipment[0], SpawnLocation);
}

void UEquipmentComponent::SpawnItemInAWorld(AWeapon* Weapon, FVector SpawnLocation)
{
	if (!Weapon || !World || !Owner->HasAuthority())
	{
		return;
	}
	World->SpawnActor<AWeapon>(Weapon->GetClass(), SpawnLocation, Owner->GetActorRotation());
	Weapon->DestroyNetworkActorHandled();
}

void UEquipmentComponent::DropAllItems()
{
	if (!World)
	{
		return;
	}
	// TODO spawn only right hand weapon
	auto ItemSpawner = GetWorld()->SpawnActor<AItemSpawner>(
												Owner->GetActorLocation(),
												Owner->GetActorRotation());
	if (Equipment[0])
	{
		ItemSpawner->SpawnItemInRandomLocation(Equipment[0]->GetClass());
		UE_LOG(LogTemp, Warning, TEXT("Try to destroy right hand weapon"))
		Equipment[0]->Destroy();
	}
	if (Equipment[1])
	{
		//ItemSpawner->SpawnItemInRandomLocation(Equipment[1]->GetClass());
		UE_LOG(LogTemp, Warning, TEXT("Try to destroy left hand weapon"))
		Equipment[1]->Destroy();
	}
	if (Equipment[2])
	{
		//ItemSpawner->SpawnItemInRandomLocation(Equipment[2]->GetClass());
		Equipment[2]->Destroy();
	}
	if (Equipment[3])
	{
		//ItemSpawner->SpawnItemInRandomLocation(Equipment[3]->GetClass());
		Equipment[3]->Destroy();
	}
	ItemSpawner->DestroyNetworkActorHandled();
}

AWeapon* UEquipmentComponent::CreateWeaponOnSocket(TSubclassOf<AWeapon> WeaponClass, 
												   FName SocketName)
{
	AWeapon* Item = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	Item->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
							FAttachmentTransformRules::SnapToTargetNotIncludingScale,
							SocketName);
	// TODO change to set owner in pick up weapon function
	Item->SetOwner(GetOwner());
	return Item;
}