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
	ServerAttackState.DamagedActors.Empty();
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
	
	Server_ClearDamagedActors();
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UEquipmentComponent, Equipment, COND_None);
	DOREPLIFETIME_CONDITION(UEquipmentComponent, ServerAttackState, COND_None);
}

void UEquipmentComponent::Server_AddDamagedActors_Implementation(const FString& ActorName)
{
	ServerAttackState.DamagedActors.Add(ActorName);
}

bool UEquipmentComponent::Server_AddDamagedActors_Validate(const FString& ActorName)
{
	return true;
}

void UEquipmentComponent::Server_ClearDamagedActors_Implementation()
{
	ServerAttackState.DamagedActors.Empty();
}

bool UEquipmentComponent::Server_ClearDamagedActors_Validate()
{
	return true;
}

void UEquipmentComponent::OnRep_ServerAttackStateChanged()
{
	if (ServerAttackState.bIsAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack status changed to  = TRUE!!!"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attack status changed to  = FALSE!!!"))
		// Clear damaged actors array after the end of attack
		if (Cast<APawn>(Owner)->IsLocallyControlled())
		{
			//UE_LOG(LogTemp, Warning, TEXT("DAMAGED array was CLEARED"));
			//Server_ClearDamagedActors();
		}
	}
}

void UEquipmentComponent::Server_SetAttack_Implementation(bool IsAttack)
{
	ServerAttackState.bIsAttack = IsAttack;
	/*if (Owner->HasAuthority() && Cast<APawn>(Owner)->IsLocallyControlled())
	{
		if (IsAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("DAMAGED array was CLEARED for SERVER"));
			Server_ClearDamagedActors();
		}
	}*/
	UE_LOG(LogTemp, Warning, TEXT("DAMAGED array was CLEARED for ALL"));
	Server_ClearDamagedActors();
}

bool UEquipmentComponent::Server_SetAttack_Validate(bool IsAttack)
{
	return true;
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

bool UEquipmentComponent::Server_EquipItem_Validate(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon)
{
	return true;
}

void UEquipmentComponent::Server_SwapWeapon_Implementation()
{
	if (Equipment[0] == nullptr || Equipment[2] == nullptr)
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
	if (!Weapon->ActorHasTag("Knife"))
	{// Spawn any weapon but not a knife
		World->SpawnActor<AWeapon>(Weapon->GetClass(), SpawnLocation, Owner->GetActorRotation());
	}
	Weapon->DestroyNetworkActorHandled();
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

void UEquipmentComponent::OnWeaponOverlap(AActor* OverlappedActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped actor: %s"), *OverlappedActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Damage check ..."));
	if (ServerAttackState.bIsAttack && 
			!ServerAttackState.DamagedActors.Contains(OverlappedActor->GetName()))
	{
		UE_LOG(LogTemp, Warning, TEXT("TAKE DAMAGE to the actor %s"), *OverlappedActor->GetName());
		TakeDamageToOverlappedActor(OverlappedActor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ACTOR %s is NOT ATTACK or IS ALREADY in damaged array"), *OverlappedActor->GetName());
	}
}

void UEquipmentComponent::TakeDamageToOverlappedActor(AActor* OverlappedActor)
{
	if (!Owner->HasAuthority())
	{
		return;
	}
	//// Create a damage event 
	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FDamageEvent DamageEvent(ValidDamageTypeClass);
	// Take damage to the overlapped actor
	OverlappedActor->TakeDamage(Equipment[0]->Damage,
								DamageEvent,
								GetOwner()->GetInstigatorController(),
								Owner);
	Server_AddDamagedActors(OverlappedActor->GetName());
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
	if (SocketName == FName("RightHandWeaponSocket"))
	{
		Item->OnWeaponOverlap.AddDynamic(this, &UEquipmentComponent::OnWeaponOverlap);
	}
	return Item;
}