// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Subject.h"
class AARPawn;
#include "ARMapSetupUI.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UARMapSetupUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void changeConfirmButtonVisibility();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void setupUIObserver(AARPawn* pawn);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* treasureButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* trapButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* confirmButton;
private:
	UFUNCTION()
	void OnHostButtonClicked()
	{
		OnConnectionButtonClicked(TREASURE_BUTTON);
	}
	UFUNCTION()
	void OnClientButtonClicked()
	{
		OnConnectionButtonClicked(TRAP_BUTTON);
	}
	UFUNCTION()
	void OnConfirmButtonClicked()
	{
		OnConnectionButtonClicked(CONFIRM_BUTTON);
	}

	void OnConnectionButtonClicked(Event event);
};
