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

	// Inner widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* StaminaBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage* Blood;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PickUpInfo = nullptr;

	// Statistics widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* StatisticSwitcher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidget* EmptyScreen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UWidget* StatisticScreen;

	//API
	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void UpdateHealthState(float CurrentHealth);
	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void UpdateStaminaState(float CurrentStamina);
	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void NotEnoughStamina();
	UFUNCTION(BlueprintCallable, Category = "PlayerStateInfo")
	void SetPickUpTipVisibility(bool Show);
	UFUNCTION(BlueprintCallable, Category = "HUD API")
	void ShowMatchStats();
	UFUNCTION(BlueprintCallable, Category = "HUD API")
	void HideMatchStats();

private:
	void StoreWidgetAnimations();
	UWidgetAnimation* const GetAnimationByName(FName AnimationName) const;
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HealthBarAnimation = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* StaminaBarAnimation = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* LowStaminaAnimation = nullptr;
};
