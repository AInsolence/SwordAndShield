// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUD_Multiplayer.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_01_API AHUD_Multiplayer : public AHUD
{
	GENERATED_BODY()

public:
    // Constructor
    AHUD_Multiplayer(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void DrawHUD() override;

    // Binding property in "meta" to prohibit compiling the HUD without appropriate widgets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    TSubclassOf<class UPlayerStateInfoWidget> PlayerStateInfoWidgetClass;

    class UPlayerStateInfoWidget* PlayerStateInfoWidget;

    /// *** API *** ///
    UFUNCTION(BlueprintCallable, Category = "HUD API")
    void UpdateHealthState(float CurrentHealth);
    UFUNCTION(BlueprintCallable, Category = "HUD API")
    void UpdateStaminaState(float CurrentStamina);
    UFUNCTION(BlueprintCallable, Category = "HUD API")
    void SetPickUpTipVisibility(bool Show);
    UFUNCTION(BlueprintCallable, Category = "HUD API")
    void NotEnoughStamina();
    UFUNCTION(BlueprintCallable, Category = "HUD API")
    void ShowMatchStats();
    UFUNCTION(BlueprintCallable, Category = "HUD API")
    void HideMatchStats();
};
