// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MovingPlatform.h"

// Sets default values
APlatformTrigger::APlatformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// create components
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("TriggerVolume");
	PressurePad = CreateDefaultSubobject<UStaticMeshComponent>("PressurePad");
	// set root and attachments
	if (ensure(TriggerVolume))
	{
		RootComponent = TriggerVolume;
	}
	if (ensure(PressurePad))
	{
		PressurePad->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (ensure(TriggerVolume))
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, 
												&APlatformTrigger::OnOverlapTriggerBegin);
		TriggerVolume->OnComponentEndOverlap.AddDynamic(this, 
												&APlatformTrigger::OnOverlapTriggerEnd);
	}
}

// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlatformTrigger::OnOverlapTriggerBegin(UPrimitiveComponent* OverlappedComponent,
												AActor* OtherActor, 
												UPrimitiveComponent* OtherComp, 
												int32 OtherBodyIndex, 
												bool bFromSweep, 
												const FHitResult& SweepResult)
{
	for (auto Platform : PlatformsToTrigger)
	{
		Platform->AddActiveTrigger();
	}
	UE_LOG(LogTemp, Warning, TEXT("Trigger is activated"));
}

void APlatformTrigger::OnOverlapTriggerEnd(UPrimitiveComponent* OverlappedComponent, 
											AActor* OtherActor, 
											UPrimitiveComponent* OtherComp, 
											int32 OtherBodyIndex)
{
	for (auto Platform : PlatformsToTrigger)
	{
		Platform->RemoveActiveTrigger();
	}
	UE_LOG(LogTemp, Warning, TEXT("Trigger is deactivated"));
}

