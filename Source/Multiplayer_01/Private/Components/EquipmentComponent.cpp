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

void UEquipmentComponent::DropWeapon()
{
	auto World = GetWorld();
	if (World)
	{
		if (Owner->HasAuthority())
		{
			if (RightHandItem)
			{
				UE_LOG(LogTemp, Warning, TEXT("TRY TO SPAWN DROPPED WEAPON"))
				/*FVector SpawnLocation = FVector(Owner->GetActorLocation().X + 200,
												Owner->GetActorLocation().Y,
												Owner->GetActorLocation().Z);*/
				FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector()*200;
				World->SpawnActor<AWeapon>(RightHandItem->GetClass(), SpawnLocation, Owner->GetActorRotation());
				RightHandItem->DestroyNetworkActorHandled();
			}
		}
	}
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