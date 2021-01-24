// Copyright © 2021 Insolence Assets. All Rights Reserved.

/***

	This class is an actor component which allows player to interact with objectson a level.
	Used "Interactable Item" interface to work with objects on the map.

***/

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/InteractableItemInterface.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UInteractionComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION(Server, Reliable, Category = "Interaction")
	void Server_PickUp();
	void Server_PickUp_Implementation();
	bool Server_PickUp_Validate();

	/** Called when a interaction sphere start overlaps something */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

	/** Called when a interaction sphere end overlapping */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, 
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp, 
						int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// API
	void Interact();
	IInteractableItemInterface* InteractableItem;

private:
	UWorld* World = nullptr;
	APawn* Owner = nullptr;
	bool bIsInteractionActivated = false;
	class UEquipmentComponent* EquipmentComponent = nullptr;
};
