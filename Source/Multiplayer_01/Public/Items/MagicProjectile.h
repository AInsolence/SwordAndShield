// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicProjectile.generated.h"

UCLASS()
class MULTIPLAYER_01_API AMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMagicProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime);

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** Explosion effect */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class UNiagaraSystem* ProjectileParticles = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class UNiagaraSystem* ExplosionParticles = nullptr;
	// Particles scale multiplier
	UPROPERTY(EditAnywhere, Category = Projectile)
	float SlashParticleScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = Projectile)
	float ImpactParticleScale = 1.0f;

	// Set damage value
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float DamageAmount = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ACharacter> InflictDamageToThisClass;

private:
	/** Called when a weapon hits something */
	UFUNCTION(BlueprintCallable, Category = "WeaponUsing")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
};
