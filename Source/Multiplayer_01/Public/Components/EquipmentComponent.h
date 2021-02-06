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

USTRUCT(BlueprintType)
struct FServerAttackState
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category = "ServerActionState")
	TArray<FString> DamagedActors;
	UPROPERTY(VisibleAnywhere, Category = "ServerActionState")
	bool bIsAttack = false;
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
	//
	UPROPERTY(ReplicatedUsing = OnRep_ServerAttackStateChanged, VisibleAnywhere, Category = "ServerAttackState")
	FServerAttackState ServerAttackState;
	UFUNCTION(BlueprintCallable, Category = "Replication")
	void OnRep_ServerAttackStateChanged();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AWeapon* GetRightHandWeapon(){ return Equipment[0]; }
	//
	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_EquipItem(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	void Server_EquipItem_Implementation(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	bool Server_EquipItem_Validate(EItemSlot ItemSlot, TSubclassOf<AWeapon> SlotWeapon);
	//
	UFUNCTION(Server, Reliable, WithValidation, Category = "Replication")
	void Server_SwapWeapon();
	void Server_SwapWeapon_Implementation();
	bool Server_SwapWeapon_Validate();
	//
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Replication")
	void Server_SetAttack(bool IsAttack);
	void Server_SetAttack_Implementation(bool IsAttack);
	bool Server_SetAttack_Validate(bool IsAttack);
	//
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Replication")
	void Server_AddDamagedActors(const FString& ActorName);
	void Server_AddDamagedActors_Implementation(const FString& ActorName);
	bool Server_AddDamagedActors_Validate(const FString& ActorName);
	//
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Replication")
	void Server_ClearDamagedActors();
	void Server_ClearDamagedActors_Implementation();
	bool Server_ClearDamagedActors_Validate();
	//
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment")
	void Server_OnWeaponOverlap(AActor* OverlappedActor);
	void Server_OnWeaponOverlap_Implementation(AActor* OverlappedActor);
	//
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Equipment")
	void Server_DropAllItems();
	void Server_DropAllItems_Implementation();
	//
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void OnNetworkFailure(UWorld* _World,
						  UNetDriver* NetDriver,
						  ENetworkFailure::Type FailureType,
						  const FString& Message);
	//
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SwapWeapon();
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void DropWeapon(FVector SpawnLocation);

	void TakeDamageToOverlappedActor(AActor* OverlappedActor);

private:
	AActor* Owner = nullptr;
	UWorld* World = nullptr;
	// Helpers
	AWeapon* CreateWeaponOnSocket(TSubclassOf<AWeapon> WeaponClass, FName SocketName);
	void SpawnItemInAWorld(AWeapon* Weapon, FVector SpawnLocation);
};
