// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYER_01_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultStamina = 100.f;
	UPROPERTY(BlueprintReadWrite, Category = "Health")
	float CurrentStamina;

	// Sprint variables
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Sprint")
	bool bIsSprinting = false;
	float BaseWalkingSpeed = 600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float BaseSprintMultiplier = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float MaxSprintMultiplier = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float TimeToMaxSprintSpeed = 2.0f;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(AActor* DamagedActor,
					float Damage,
					const class UDamageType* DamageType,
					class AController* InstigatedBy,
					AActor* DamageCauser);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE float GetDefaultHealth() const { return DefaultHealth; };
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; };
	FORCEINLINE float GetDefaultStamina() const { return DefaultStamina; };
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; };

	UFUNCTION(BlueprintCallable)
	void ChangeCurrentStaminaTo(float StaminaCost);
	UFUNCTION(BlueprintCallable)
	void SetIsSprinting(bool IsSprinting)
	{
		bIsSprinting = IsSprinting;
		if (bIsSprinting)
		{
			DrawDebugString(GetWorld(),
				FVector(0, 0, 30),
				"RUN",
				GetOwner(),
				FColor::Blue,
				1.0f);
		}
	}

private:
	class ACharacter* Owner = nullptr;
};
