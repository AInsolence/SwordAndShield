// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableItemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MULTIPLAYER_01_API IInteractableItemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void SetOwner(AActor* Owner) = 0;
	virtual AActor* GetOwner() = 0;
	virtual void PickUp() = 0;
	virtual void Use() = 0;
	virtual class USkeletalMeshComponent* GetItemMesh() = 0;
	virtual class UImage* GetItemImage() = 0;
};
