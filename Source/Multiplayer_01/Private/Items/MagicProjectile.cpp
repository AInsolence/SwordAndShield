// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/MagicProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/DamageType.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"

// Sets default values
AMagicProjectile::AMagicProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 60;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(20.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMagicProjectile::OnHit);// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AMagicProjectile::Tick(float DeltaTime)
{
	if (ProjectileParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ProjectileParticles, GetActorLocation());
	}
}

void AMagicProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("HIT 1"))
	if (ExplosionParticles) //bIsExplosiveRound && ExplosionParticles)
	{
		// create explosion particle effect
		auto Explosion = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionParticles, GetActorLocation());
		if (true) //InflictDamageToThisClass)
		{
			if (true) //OtherActor->GetClass()->IsChildOf(InflictDamageToThisClass))
			{
				UE_LOG(LogTemp, Warning, TEXT("HIT 2"))
				// Create a damage event  
				TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				FDamageEvent DamageEvent(ValidDamageTypeClass);

				if (GetWorld())
				{
					if (GetWorld()->GetFirstPlayerController())
					{
						UE_LOG(LogTemp, Warning, TEXT("HIT to: %s"), *OtherActor->GetName())
						OtherActor->TakeDamage(DamageAmount,
												DamageEvent,
												GetWorld()->GetFirstPlayerController(),
												this);
					}
				}
			}
		}
		// destroy projectile
		Destroy();
	}
}