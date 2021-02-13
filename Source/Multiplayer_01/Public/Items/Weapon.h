// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InteractableItemInterface.h"
#include "Net/UnrealNetwork.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponOverlap, AActor*, OverlappedActor);

UCLASS()
class MULTIPLAYER_01_API AWeapon : public AActor, public IInteractableItemInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWeapon();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Weapon Stats
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

	// Interactable interface implementation
	virtual void SetOwner(AActor* _Owner);
	virtual AActor* GetOwner();
	virtual UClass* PickUp() override;
	virtual void Use() override;
	virtual class USkeletalMeshComponent* GetItemMesh() override;
	virtual class UImage* GetItemImage() override;

	/** Called when a weapon hits something */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "WeaponUsing")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

	void OnOverlapBegin_Implementation(UPrimitiveComponent* OverlappedComponent,
									   AActor* OtherActor,
									   UPrimitiveComponent* OtherComp,
									   int32 OtherBodyIndex,
									   bool bFromSweep,
									   const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "WeaponUsing")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
					  AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp,
					  int32 OtherBodyIndex);
	
	void OnOverlapEnd_Implementation(UPrimitiveComponent* OverlappedComponent,
									 AActor* OtherActor,
									 UPrimitiveComponent* OtherComp,
									 int32 OtherBodyIndex);
					  
	void GetOverlappedEnemy(AActor* OtherActor);
	//
	FOnWeaponOverlap OnWeaponOverlap;

	// Weapon components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponents")
	class USkeletalMeshComponent* WeaponMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponents")
	class UBoxComponent* DamageCollisionBox = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponComponents")
	class UBoxComponent* PickUpCollisionBox = nullptr;

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
	
};
