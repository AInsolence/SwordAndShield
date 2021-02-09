// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AudioFXComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EquipmentComponent.h"

// Sets default values for this component's properties
UAudioFXComponent::UAudioFXComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UAudioFXComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (Owner)
	{
		auto EquipmentCompObject = GetOwner()->GetDefaultSubobjectByName("EquipmentComponent");
		if (EquipmentCompObject)
		{
			EquipmentComponent = Cast<UEquipmentComponent>(EquipmentCompObject);
		}
	}
}

void UAudioFXComponent::Server_PlaySoundFX_Implementation(USoundBase* SoundFX)
{
	if (!SoundFX)
	{
		UE_LOG(LogTemp, Warning, TEXT("Audio component: no sound effect"));
		return;
	}
	Multicast_PlaySoundFX(SoundFX);
}

void UAudioFXComponent::Multicast_PlaySoundFX_Implementation(USoundBase* SoundFX)
{
	// play sound
	if (Owner && SoundFX)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundFX, Owner->GetActorLocation());
	}
}

void UAudioFXComponent::Blocked()
{
	if (EquipmentComponent)
	{
		auto BlockSound = EquipmentComponent->Equipment[1]->SoundFX_Block_01;
		Server_PlaySoundFX(BlockSound);
	}
}

void UAudioFXComponent::PickUp()
{
	if (EquipItem_AudioFX)
	{
		Server_PlaySoundFX(EquipItem_AudioFX);
	}
}

void UAudioFXComponent::Death(AController* InstigatedBy)
{
	if (Death_AudioFX)
	{
		Server_PlaySoundFX(Death_AudioFX);
	}
}
