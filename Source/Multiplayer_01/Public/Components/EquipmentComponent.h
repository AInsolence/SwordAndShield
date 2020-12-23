// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Weapon.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TEST Weapon")
	AWeapon* RightHandItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TEST Weapon")
	AWeapon* LeftHandItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TEST Weapon")
	AWeapon* BeltPlaceItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TEST Weapon")
	AWeapon* BackPlaceItem;

	void EquipItem(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	void SwapWeapon();
	void DropWeapon(FVector SpawnLocation);

private:
	AWeapon* CreateWeaponOnSocket(TSubclassOf<AWeapon> WeaponClass, FName SocketName);
	AActor* Owner = nullptr;
};
