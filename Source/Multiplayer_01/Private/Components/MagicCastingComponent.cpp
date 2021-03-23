// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MagicCastingComponent.h"
#include "Items/MagicProjectile.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMagicCastingComponent::UMagicCastingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(false);
}


void UMagicCastingComponent::Server_CreateMagicProjectile_Implementation(TSubclassOf<class AMagicProjectile> Projectile)
{
	if (Projectile != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			auto Mesh = Cast<ACharacter>(GetOwner())->GetMesh();
			if (Mesh)
			{
				FVector MagicCastingLocation = Mesh->GetSocketLocation("SpecialAttackSocket");
				FRotator MagicCastingRotation = GetOwner()->GetActorForwardVector().Rotation();
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				// spawn the projectile at the place
				auto MagicProj = World->SpawnActor<AMagicProjectile>(
																	Projectile,
																	MagicCastingLocation,
																	MagicCastingRotation,
																	ActorSpawnParams);
				MagicProj->SetOwner(GetOwner());
				// Play magic cast audio
				OnMagicCast.Broadcast(MagicCast_AudioFX);
			}
		}
	}
}

// Called when the game starts
void UMagicCastingComponent::BeginPlay()
{
	Super::BeginPlay();	
}

