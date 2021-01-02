// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API UPlayerStateInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Constructor
	UPlayerStateInfoWidget(const FObjectInitializer& ObjectInitializer);
	// Native constructor
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* StaminaBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* Blood;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PickUpInfo = nullptr;

	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void UpdateHealthState(float CurrentHealth);
	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void UpdateStaminaState(float CurrentStamina);
	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void SetPickUpTipVisibility(bool Show);

private:
	void StoreWidgetAnimations();
	UWidgetAnimation* const GetAnimationByName(FName AnimationName) const;
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HealthBarAnimation = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* StaminaBarAnimation = nullptr;
};
