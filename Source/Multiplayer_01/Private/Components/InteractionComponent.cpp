// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InteractionComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	Owner = Cast<APawn>(GetOwner());

	this->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnBeginOverlap);
	this->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnEndOverlap);
}

void UInteractionComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
											AActor* OtherActor,
											UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex, bool bFromSweep,
											const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		auto _InteractableItem = Cast<IInteractableItemInterface>(OtherActor);
		if (_InteractableItem)
		{
			if (_InteractableItem->GetOwner() == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Overlapped item"))
				// TODO show hint about item
				InteractableItem = _InteractableItem;
			}
		}
	}
}

void UInteractionComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
											AActor* OtherActor,
											UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex)
{
	InteractableItem = nullptr;
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractionComponent::Interact()
{
	if (InteractableItem)
	{
		InteractableItem->PickUp();
	}
}
