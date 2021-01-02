// Fill out your copyright notice in the Description page of Project Settings.

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	//
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BaseTestWeapon;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BaseTestShield;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BeltTestWeapon;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<AWeapon> BackTestWeapon;

public:
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "TEST Weapon")
	TArray<AWeapon*> Equipment;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AWeapon* GetRightHandWeapon()
	{
		return Equipment[0];
	}

	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_EquipItem(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	void Server_EquipItem_Implementation(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	bool Server_EquipItem_Validate(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_SwapWeapon();
	void Server_SwapWeapon_Implementation();
	bool Server_SwapWeapon_Validate();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SwapWeapon();
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DropWeapon(FVector SpawnLocation);
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DropAllItems();

private:
	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	//
	AWeapon* CreateWeaponOnSocket(TSubclassOf<AWeapon> WeaponClass, FName SocketName);
	void SpawnItemInAWorld(AWeapon* Weapon, FVector SpawnLocation);
};
