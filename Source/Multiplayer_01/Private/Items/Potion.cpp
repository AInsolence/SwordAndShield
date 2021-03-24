// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potion.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Multiplayer_01/Multiplayer_01Character.h"
#include "Components/HealthComponent.h"
#include "Components/ManaComponent.h"
#include "Components/StaminaComponent.h"

// Sets default values
APotion::APotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	NetUpdateFrequency = 2;

	PickUpCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PickUpCollisionBox"));
	SetRootComponent(PickUpCollisionBox);
	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotionMesh"));
	if (PickUpCollisionBox && PotionMesh)
	{
		PotionMesh->SetupAttachment(PickUpCollisionBox);
	}
}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
	PickUpCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APotion::OnOverlapBegin);
	GetWorld()->GetTimerManager().SetTimer(TimerHandler,
											[this]()
											{
												PotionAnimation();
											},
											1.f/TimesInSecond,
											true);
}

void APotion::PotionAnimation()
{
	AddActorLocalRotation(FRotator(Roll, Pitch, Yaw));
}


void APotion::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
							 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
							 bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (OtherActor)
	{
		auto Character = Cast<AMultiplayer_01Character>(OtherActor);
		if (Character)
		{
			if (HealthPoints)
			{
				auto HealthComponent = Character->HealthComponent;
				if (HealthComponent)
				{
					HealthComponent->ChangeCurrentHealthTo(HealthPoints);
				}
			}
			if (ManaPoints)
			{
				auto ManaComponent = Character->ManaComponent;
				if (ManaComponent)
				{
					ManaComponent->ChangeCurrentManaTo(ManaPoints);
				}
			}
			if (StaminaPoints)
			{
				auto StaminaComponent = Character->StaminaComponent;
				if (StaminaComponent)
				{
					StaminaComponent->ChangeCurrentStaminaTo(StaminaPoints);
				}
			}
			// Destroy potion after pick up
			Destroy();
		}
	}
}
