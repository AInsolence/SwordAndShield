// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InteractableItemInterface.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"

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

	//TODO Change to appropriate after test
	if (BaseTestWeapon && BaseTestShield)
	{
		EquipItem(EItemSlot::RightHandItem, BaseTestWeapon);
		EquipItem(EItemSlot::LeftHandItem, BaseTestShield);
		if (BeltTestWeapon && BackTestWeapon)
		{
			EquipItem(EItemSlot::BeltPlaceItem, BeltTestWeapon);
			EquipItem(EItemSlot::BackPlaceItem, BackTestWeapon);
		}
	}
}

void UEquipmentComponent::EquipItem(EItemSlot ItemSlot, TSubclassOf<class AWeapon> SlotWeapon)
{
	if (!SlotWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT slot weapon"))
		return;
	}

	switch (ItemSlot)
	{
		case EItemSlot::RightHandItem:
			if (RightHandItem)
			{
				RightHandItem->Destroy();
				RightHandItem = nullptr;
			}
			RightHandItem = CreateWeaponOnSocket(SlotWeapon, "RightHandWeaponSocket");
			break;
		case EItemSlot::LeftHandItem:
			LeftHandItem = CreateWeaponOnSocket(SlotWeapon, "LeftHandWeaponSocket");
			break;
		case EItemSlot::BeltPlaceItem:
			BeltPlaceItem = CreateWeaponOnSocket(SlotWeapon, "BeltWeaponSocket");
			break;
		case EItemSlot::BackPlaceItem:
			BackPlaceItem = CreateWeaponOnSocket(SlotWeapon, "BackWeaponSocket");
			break;
		default:
			break;
	}
}

void UEquipmentComponent::SwapWeapon()
{
	if (!RightHandItem || !BeltPlaceItem)
	{
		return;
	}
	auto TempWeapon = RightHandItem->GetClass();
	//
	RightHandItem->Destroy();
	RightHandItem = CreateWeaponOnSocket(BeltPlaceItem->GetClass(), "RightHandWeaponSocket");
	//
	BeltPlaceItem->Destroy();
	BeltPlaceItem = CreateWeaponOnSocket(TempWeapon, "BeltWeaponSocket");
}

void UEquipmentComponent::DropWeapon(FVector SpawnLocation)
{
	SpawnItemInAWorld(RightHandItem, SpawnLocation);
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