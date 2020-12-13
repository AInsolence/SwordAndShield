// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Components/InteractableItemInterface.h"
#include "GameFramework/Character.h"

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

// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEquipmentComponent::EquipItem(EItemSlot ItemSlot, TSubclassOf<class AWeapon> SlotWeapon)
{
	switch (ItemSlot)
	{
		case EItemSlot::RightHandItem:
			RightHandItem = GetWorld()->SpawnActor<AWeapon>(SlotWeapon);
			RightHandItem->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
							FAttachmentTransformRules::SnapToTargetNotIncludingScale,
							FName("RightHandWeaponSocket"));
			RightHandItem->WeaponOwner = GetOwner();
			break;
		case EItemSlot::LeftHandItem:
			LeftHandItem = GetWorld()->SpawnActor<AWeapon>(SlotWeapon);
			LeftHandItem->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
							FAttachmentTransformRules::SnapToTargetNotIncludingScale,
							FName("LeftHandWeaponSocket"));
			LeftHandItem->WeaponOwner = GetOwner();
			break;
		case EItemSlot::BeltPlaceItem:
			BeltPlaceItem = GetWorld()->SpawnActor<AWeapon>(SlotWeapon);
			BeltPlaceItem->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
							FAttachmentTransformRules::SnapToTargetNotIncludingScale,
							FName("BeltWeaponSocket"));
			break;
		case EItemSlot::BackPlaceItem:
			BackPlaceItem = GetWorld()->SpawnActor<AWeapon>(SlotWeapon);
			BackPlaceItem->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				FName("BackWeaponSocket"));
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

	auto TempWeapon = RightHandItem;

	RightHandItem->Destroy();
	RightHandItem = GetWorld()->SpawnActor<AWeapon>(BeltPlaceItem->GetClass());
	RightHandItem->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
					FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					FName("RightHandWeaponSocket"));
	RightHandItem->WeaponOwner = GetOwner();

	BeltPlaceItem->Destroy();
	BeltPlaceItem = GetWorld()->SpawnActor<AWeapon>(TempWeapon->GetClass());
	BeltPlaceItem->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(),
					FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					FName("BeltWeaponSocket"));
}

