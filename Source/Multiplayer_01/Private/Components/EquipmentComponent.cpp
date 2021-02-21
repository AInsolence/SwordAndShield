// Copyright © 2021 Insolence Assets. All Rights Reserved.

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

	Equipment.Init(nullptr, 4);
}

void UEquipmentComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	
	if (Equipment.Num() > 0)
	{
		for (auto Item : Equipment)
		{
			if (Item)
			{// Destroy equipment items after component destroying
				Item->Destroy();
			}
		}
	}
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	World = GetWorld();

	//TODO Change to appropriate after test
	if (BaseTestWeapon)
	{
		Server_EquipItem(EItemSlot::RightHandItem, BaseTestWeapon);
	}
	if (BaseTestShield)
	{
		Server_EquipItem(EItemSlot::LeftHandItem, BaseTestShield);
	}
	if (BeltTestWeapon)
	{
		Server_EquipItem(EItemSlot::BeltPlaceItem, BeltTestWeapon);
	}
	if (BackTestWeapon)
	{
		Server_EquipItem(EItemSlot::BackPlaceItem, BackTestWeapon);
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

void UEquipmentComponent::SwapWeapon(uint8 FirstWeaponIndex, uint8 SecondWeaponIndex)
{
	if (FirstWeaponIndex < Equipment.Num() ||
		SecondWeaponIndex < Equipment.Num())
	{
		Server_SwapWeapon(FirstWeaponIndex, SecondWeaponIndex);
	}
}

void UEquipmentComponent::Server_SwapWeapon_Implementation(uint8 FirstWeaponIndex, uint8 SecondWeaponIndex)
{
	if (Equipment[FirstWeaponIndex] == nullptr || Equipment[SecondWeaponIndex] == nullptr)
	{
		return;
	}
	FName FirstSocketName;
	FName SecondSocketName;
	
	FirstSocketName = FindSocketNameByIndex(FirstWeaponIndex);
	SecondSocketName = FindSocketNameByIndex(SecondWeaponIndex);

	auto TempWeapon = Equipment[FirstWeaponIndex]->GetClass();
	//
	Equipment[FirstWeaponIndex]->Destroy();
	Equipment[FirstWeaponIndex] = CreateWeaponOnSocket(Equipment[SecondWeaponIndex]->GetClass(), FirstSocketName);
	//
	Equipment[SecondWeaponIndex]->Destroy();
	Equipment[SecondWeaponIndex] = CreateWeaponOnSocket(TempWeapon, SecondSocketName);
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
	if (!Weapon->ActorHasTag("Knife"))
	{// Spawn any weapon but not a knife
		World->SpawnActor<AWeapon>(Weapon->GetClass(), SpawnLocation, Owner->GetActorRotation());
	}
	Weapon->DestroyNetworkActorHandled();
}

void UEquipmentComponent::Server_DropAllItems_Implementation(AController* InstigatedBy)
{
	DropAllItems();
}

void UEquipmentComponent::DropAllItems()
{
	if (!World)
	{
		return;
	}
	//
	auto ItemSpawner = GetWorld()->SpawnActor<AItemSpawner>(Owner->GetActorLocation(),
												            Owner->GetActorRotation());
	if (Equipment[0])
	{// Spawn any weapon but not a knife
		if (!Equipment[0]->ActorHasTag("Knife"))
		{
			ItemSpawner->SpawnItemInRandomLocation(Equipment[0]->GetClass());			
		}
		Equipment[0]->Destroy();
	}
	if (Equipment[1]) // TODO would be changed to spawn all player's equipment
	{
		Equipment[1]->Destroy();
	}
	if (Equipment[2])
	{
		if (!Equipment[2]->ActorHasTag("Knife"))
		{// Spawn any weapon but not a knife
			ItemSpawner->SpawnItemInRandomLocation(Equipment[2]->GetClass());
		}
		Equipment[2]->Destroy();
	}
	if (Equipment[3])
	{
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
	// Set weapon owner
	Item->SetOwner(GetOwner());
	return Item;
}

FName UEquipmentComponent::FindSocketNameByIndex(uint8 Index)
{
	switch (Index)
	{
	case 0:
		return "RightHandWeaponSocket";
		break;
	case 1:
		return "LeftHandWeaponSocket";
		break;
	case 2:
		return "BeltWeaponSocket";
		break;
	case 3:
		return "BackWeaponSocket";
		break;
	default:
		return "";
		break;
	}
}