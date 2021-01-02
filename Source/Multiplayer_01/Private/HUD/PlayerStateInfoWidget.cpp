// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerStateInfoWidget.h"
#include "Runtime/UMG/Public/UMG.h"

UPlayerStateInfoWidget::UPlayerStateInfoWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UPlayerStateInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StoreWidgetAnimations();

	HealthBarAnimation = GetAnimationByName(FName("HealthBarAnimation"));
	StaminaBarAnimation = GetAnimationByName(FName("StaminaBarAnimation"));

	if (HealthBar && StaminaBar && Blood && PickUpInfo)
	{// show if hidden
		if (HealthBar->Visibility == ESlateVisibility::Hidden)
		{
			HealthBar->SetVisibility(ESlateVisibility::Visible);
		}
		if (StaminaBar->Visibility == ESlateVisibility::Hidden)
		{
			StaminaBar->SetVisibility(ESlateVisibility::Visible);
		}
		if (Blood->Visibility == ESlateVisibility::Hidden)
		{
			Blood->SetVisibility(ESlateVisibility::Visible);
		}
		PickUpInfo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerStateInfoWidget::UpdateHealthState(float CurrentHealth)
{
	HealthBar->SetPercent(CurrentHealth);
	if (HealthBarAnimation)
	{
		PlayAnimation(HealthBarAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0, false);
	}
	Blood->SetOpacity(0.7f - CurrentHealth);
}

void UPlayerStateInfoWidget::UpdateStaminaState(float CurrentStamina)
{
	StaminaBar->SetPercent(CurrentStamina);
	if (StaminaBarAnimation)
	{
		PlayAnimation(StaminaBarAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0, false);
	}
}

void UPlayerStateInfoWidget::SetPickUpTipVisibility(bool Show)
{
	if (Show)
	{
		PickUpInfo->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		PickUpInfo->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerStateInfoWidget::StoreWidgetAnimations()
{
	// clear map before filling
	AnimationsMap.Empty();

	// get all properties from this class
	UProperty* Property = GetClass()->PropertyLink;
	while (Property)
	{
		// only deal with object properties
		if (Property->GetClass() == UObjectProperty::StaticClass())
		{// cast all properties to object property
			UObjectProperty* ObjectProperty = Cast<UObjectProperty>(Property);
			// select only widget animation props
			if (ObjectProperty->PropertyClass == UWidgetAnimation::StaticClass())
			{// get object from property
				UObject* AnimObject = ObjectProperty->GetObjectPropertyValue_InContainer(this);
				// cast to widget animation object
				UWidgetAnimation* Animation = Cast<UWidgetAnimation>(AnimObject);
				// if exist and has a movie scene
				if (Animation && Animation->MovieScene)
				{// extract animation name
					FName AnimName = Animation->MovieScene->GetFName();
					// add animation name and animation itself to the map
					AnimationsMap.Add(AnimName, Animation);
				}
			}
		}
		Property = Property->PropertyLinkNext;
	}
}

UWidgetAnimation* const UPlayerStateInfoWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* Widgetanimation= AnimationsMap.Find(AnimationName);
	if (Widgetanimation)
	{
		return *Widgetanimation;
	}
	return nullptr;
}
