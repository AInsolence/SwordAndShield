// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerStateInfoWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "HUD/ScoreTableRowWidget.h"
#include "CustomPlayerState.h"

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
	LowStaminaAnimation = GetAnimationByName(FName("LowStaminaAnimation"));
	ManaBarAnimation = GetAnimationByName(FName("ManaBarAnimation"));
	LowManaAnimation = GetAnimationByName(FName("LowManaAnimation"));

	if (HealthBar && StaminaBar && Blood && PickUpInfo && ManaBar)
	{// show if hidden
		if (HealthBar->Visibility == ESlateVisibility::Hidden)
		{
			HealthBar->SetVisibility(ESlateVisibility::Visible);
		}
		if (ManaBar->Visibility == ESlateVisibility::Hidden)
		{
			ManaBar->SetVisibility(ESlateVisibility::Visible);
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

void UPlayerStateInfoWidget::UpdateManaState(float CurrentMana)
{
	// Find stamina cost
	float Delta = ManaBar->Percent - CurrentMana;
	// Set current stamina
	ManaBar->SetPercent(CurrentMana);
	// Animate action's stamina consumption
	if (ManaBarAnimation && FMath::Abs(Delta) > 0.02f)
	{
		PlayAnimation(ManaBarAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0, false);
	}
}

void UPlayerStateInfoWidget::NotEnoughMana()
{
	if (LowManaAnimation)
	{
		PlayAnimation(LowManaAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0, false);
	}
}

void UPlayerStateInfoWidget::UpdateStaminaState(float CurrentStamina)
{
	// Find stamina cost
	float Delta = StaminaBar->Percent - CurrentStamina;
	// Set current stamina
	StaminaBar->SetPercent(CurrentStamina);
	// Animate action's stamina consumption
	if (StaminaBarAnimation && FMath::Abs(Delta) > 0.02f)
	{
		PlayAnimation(StaminaBarAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0, false);
	}
}

void UPlayerStateInfoWidget::NotEnoughStamina()
{
	if (LowStaminaAnimation)
	{
		PlayAnimation(LowStaminaAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0, false);
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

void UPlayerStateInfoWidget::ShowMatchStats()
{
	if (StatisticSwitcher)
	{
		UpdateScoreTable();
		StatisticSwitcher->SetActiveWidget(StatisticScreen);
	}
}

void UPlayerStateInfoWidget::HideMatchStats()
{
	if (StatisticSwitcher)
	{
		StatisticSwitcher->SetActiveWidget(EmptyScreen);
	}
}

void UPlayerStateInfoWidget::StoreWidgetAnimations()
{
	// clear map before filling
	AnimationsMap.Empty();

	// get all properties from this class
	FProperty* Property = GetClass()->PropertyLink;
	while (Property)
	{
		// only deal with object properties
		if (Property->GetClass() == FObjectProperty::StaticClass())
		{// cast all properties to object property
			FObjectProperty* ObjectProperty = CastFieldChecked<FObjectProperty>(Property);
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

void UPlayerStateInfoWidget::UpdateScoreTable()
{
	if (!ScoreTableRowWidgetClass)
	{
		return;
	}
	auto GameState = GetWorld() != nullptr ? GetWorld()->GetGameState() : nullptr;
	if (GameState)
	{
		ScoreTable->ClearChildren();
		TArray<ACustomPlayerState*> PlayersStateArray;
		for (auto state : GameState->PlayerArray)
		{
			PlayersStateArray.Add(Cast<ACustomPlayerState>(state));
		}
		PlayersStateArray.Sort([](const ACustomPlayerState& A, const ACustomPlayerState& B) -> bool
		{
			if (A.GetScore() > B.GetScore())
			{
				return true;
			}
			else if (A.GetScore() == B.GetScore())
			{
				return A.GetDeaths() < B.GetDeaths();
			}
			else
			{
				return false;
			}
		});
		for (auto State : PlayersStateArray)
		{
			auto PlayerState = Cast<ACustomPlayerState>(State);
			UScoreTableRowWidget* ScoreTableRow = CreateWidget<UScoreTableRowWidget>(this, ScoreTableRowWidgetClass);
			if (ScoreTableRow && ScoreTable)
			{
				ScoreTableRow->SetData(PlayerState->GetPlayerName(), PlayerState->GetScore(), PlayerState->GetDeaths());
				ScoreTable->AddChild(ScoreTableRow);
				// Set color for self row in a table
				if (PlayerState == GetWorld()->GetFirstPlayerController()->GetPawn()->GetPlayerState())
				{
					UE_LOG(LogTemp, Warning, TEXT("Local player state here"))
					ScoreTableRow->SetColor(FColor::Emerald);
				}
			}
		}
	}
}
