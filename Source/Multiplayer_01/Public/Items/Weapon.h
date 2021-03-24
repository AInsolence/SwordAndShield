// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class is an actor represents a weapon base used for all weapons in the game.

***/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InteractableItemInterface.h"
#include "Components/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon.generated.h"

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

UCLASS()
class MULTIPLAYER_01_API AWeapon : public AActor, public IInteractableItemInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Weapon stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float Damage_Attack01 = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float Damage_Attack02 = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float SpeedOfAttack01 = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float SpeedOfAttack02 = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float StaminaCost_Attack01 = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float StaminaCost_Attack02 = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	TSubclassOf<class AMagicProjectile> MagicProjectile;

	// Weapon using animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* UseAnimation_01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* UseAnimation_02;

	// SoundFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class USoundBase* SoundFX_Attack_01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class USoundBase* SoundFX_Attack_02;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class USoundBase* SoundFX_Block_01;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class USoundBase* Hit_SoundFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class USoundBase* Unshealth_SoundFX;

	/** API */
	// Interactable interface implementation
	virtual void SetOwner(AActor* _Owner);
	virtual AActor* GetOwner() override;
	virtual UClass* PickUp() override;
	virtual void Use() override;
	virtual class USkeletalMeshComponent* GetItemMesh() override;
	virtual class UImage* GetItemImage() override;
	// Get current action type
	EActionType GetOwnerActionType();
	// Get stamina cost depends on attack type
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetAttackStaminaCost();
	// Get damage depends on attack type
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetCurrentAttackDamage();
	/** API END*/

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replication")
	void Server_SetAttack(bool IsAttack);
	void Server_SetAttack_Implementation(bool IsAttack);

	/** Called when a weapon hits something */
	UFUNCTION(BlueprintCallable, Category = "WeaponUsing")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "WeaponUsing")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Weapon components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponents")
	class USkeletalMeshComponent* WeaponMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponents")
	class UBoxComponent* DamageCollisionBox = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponents")
	class UBoxComponent* PickUpCollisionBox = nullptr;

private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "ServerAttackState")
	FServerAttackState ServerAttackState;
	//
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replication")
	void Server_AddDamagedActors(const FString& ActorName);
	void Server_AddDamagedActors_Implementation(const FString& ActorName);
	//
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replication")
	void Server_ClearDamagedActors();
	void Server_ClearDamagedActors_Implementation();
	//
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Replication")
	void TakeDamageToOverlappedActor(AActor* OverlappedActor);
	void TakeDamageToOverlappedActor_Implementation(AActor* OverlappedActor);
	//
	void GetOverlappedEnemy(AActor* OtherActor);
	void OnWeaponOverlap(AActor* OverlappedActor);
};
