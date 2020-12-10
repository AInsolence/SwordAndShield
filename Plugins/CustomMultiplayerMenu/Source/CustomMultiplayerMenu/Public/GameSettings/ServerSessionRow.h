// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerSessionRow.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMMULTIPLAYERMENU_API UServerSessionRow : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UTextBlock* SessionName;
	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UTextBlock* HostPlayerName;
	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UTextBlock* PlayersCount;
	UPROPERTY(EditAnywhere, Category = "ServerSessionRow", meta = (BindWidget))
	class UButton* SessionRowButton;

	void Setup(class UMultiplayerMenuWidget* Parent, uint32 SessionIndex);
	UFUNCTION()
	void OnSessionRowButtonClicked();
	UFUNCTION()
	void OnSessionRowButtonOnHovered();
	UFUNCTION()
	void OnSessionRowButtonOnUnhovered();

	void SetAsSelected(bool IsSelected);
	FORCEINLINE uint32 GetSessionIndex() const {return SessionIndex;}

private:

	UPROPERTY(VisibleAnywhere, Category = "ServerSessionRow")
	class UMultiplayerMenuWidget* Parent;
	uint32 SessionIndex;

	FLinearColor UnhoveredColor = FLinearColor::White;
	FLinearColor HoveredColor = FLinearColor::Yellow;

	void SetRowTextColor(FLinearColor Color);
};
