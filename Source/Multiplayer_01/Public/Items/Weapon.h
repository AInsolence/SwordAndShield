// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InteractableItemInterface.h"
#include "Weapon.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponStats")
	float Damage = 30.0f;

	// Interactable interface implementation
	virtual void SetOwner(AActor* _Owner);
	virtual AActor* GetOwner();
	virtual void PickUp() override;
	virtual void Use() override;
	virtual class USkeletalMeshComponent* GetItemMesh() override;
	virtual class UImage* GetItemImage() override;
	//
	UFUNCTION(BlueprintCallable, Category = "WeaponUsing")
	void Deactivate()
	{
		bActivated = false;
	}
	/** Called when a weapon hits something */
	UFUNCTION(BlueprintCallable, Category = "WeaponUsing")
	void OnOverlap(UPrimitiveComponent* OverlappedComponent,
					AActor* OtherActor,
					UPrimitiveComponent* OtherComp,
					int32 OtherBodyIndex,
					bool bFromSweep,
					const FHitResult& SweepResult);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* UseAnimation_03;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	class UAnimMontage* UseAnimation_04;

private:
	class AActor* WeaponOwner = nullptr;
	bool bActivated = false;
};
