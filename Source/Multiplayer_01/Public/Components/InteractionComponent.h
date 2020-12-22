// Fill out your copyright notice in the Description page of Project Settings.

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
	void Interact();
private:
	UWorld* World = nullptr;
	APawn* Owner = nullptr;
	IInteractableItemInterface* InteractableItem;
	bool bIsInteractionActivated = false;
};
