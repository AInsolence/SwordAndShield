// Copyright � 2021 Insolence Assets. All Rights Reserved.

#include "Components/InteractionComponent.h"
#include "Components/EquipmentComponent.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	SetRelativeScale3D(FVector(3.5f));
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	Owner = Cast<APawn>(GetOwner());
	if (Owner)
	{
		auto EquipmentCompObject = GetOwner()->GetDefaultSubobjectByName("EquipmentComponent");
		if (EquipmentCompObject)
		{
			EquipmentComponent = Cast<UEquipmentComponent>(EquipmentCompObject);
		}
	}

	this->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnBeginOverlap);
	this->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnEndOverlap);
}

void UInteractionComponent::Server_PickUp_Implementation()
{
	if (InteractableItem && EquipmentComponent)
	{
		if (InteractableItem->GetOwner() != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot pickup cause of owner"))
			return;
		}
		auto ItemClass = InteractableItem->PickUp();
		if (ItemClass->IsChildOf(AWeapon::StaticClass()))
		{
			OnPickUp.Broadcast();
			FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100;
			EquipmentComponent->DropWeapon(SpawnLocation);
			EquipmentComponent->Server_EquipItem(EItemSlot::RightHandItem, ItemClass);
		}
	}
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
			if (!_InteractableItem->GetOwner())
			{
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
	Server_PickUp();
}
