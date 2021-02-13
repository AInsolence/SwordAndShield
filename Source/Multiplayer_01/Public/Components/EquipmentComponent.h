// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class is an actor component represents player's equipment (without inventory) and
	an API to equip, swap and drop weapon.

***/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "EquipmentComponent.generated.h"

UENUM(BlueprintType)
enum class EItemSlot : uint8
{
	RightHandItem,
	LeftHandItem,
	BeltPlaceItem,
	BackPlaceItem
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();
	// Destroy all items if clien disconnects
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	// Weapons array
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "TEST Weapon")
	TArray<AWeapon*> Equipment;

	// API
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AWeapon* GetRightHandWeapon(){ return Equipment[0]; }
	//
	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_EquipItem(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	void Server_EquipItem_Implementation(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	bool Server_EquipItem_Validate(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	// Swap item(s)
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SwapWeapon();
	//
	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_SwapWeapon();
	void Server_SwapWeapon_Implementation();
	bool Server_SwapWeapon_Validate();
	// Drop item(s)
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DropWeapon(FVector SpawnLocation);
	//
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DropAllItems();
	//
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment")
	void Server_DropAllItems();
	void Server_DropAllItems_Implementation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// Start weapons
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BaseTestWeapon;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BaseTestShield;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BeltTestWeapon;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BackTestWeapon;

private:
	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	// Helpers
	AWeapon* CreateWeaponOnSocket(TSubclassOf<AWeapon> WeaponClass, FName SocketName);
	void SpawnItemInAWorld(AWeapon* Weapon, FVector SpawnLocation);
};
