// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	class AWeapon* RightHandItem;
	class AWeapon* LeftHandItem;
	class AWeapon* BeltPlaceItem;
	class AWeapon* BackPlaceItem;

	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<class AWeapon> BaseTestWeapon;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<class AWeapon> BaseTestShield;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<class AWeapon> BeltTestWeapon;
	UPROPERTY(EditAnywhere, Category = "TEST Weapon")
	TSubclassOf<class AWeapon> BackTestWeapon;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void EquipItem(EItemSlot ItemSlot, TSubclassOf<class AWeapon> SlotWeapon);
};
