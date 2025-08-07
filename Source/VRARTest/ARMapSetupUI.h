// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "ARButtonWidget.h"
#include "Components/Border.h"
#include "HealthBarWidget.h"
#include "Subject.h"
class UUIObserver;
class AARPawn;
#include "ARMapSetupUI.generated.h"

UCLASS()
class VRARTEST_API UARMapSetupUI : public UUserWidget
{

	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void changeButtonVisibility(UButton* button);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void setupUIObserver(AARPawn* pawn);

	UButton* getConfirmButton()
	{
		return confirmButton;
	}

	void resetObjectType()
	{
		currentlySelectedButtonType = EEvent::EMPTY;
		moveButtons(currentlySelectedButtonType);
	}

	UHealthBarWidget* getDwarfHealthBar()
	{
		return dwarfHealthBar;
	}

	UHealthBarWidget* getOtherHealthBar()
	{
		return otherHealthBar;
	}

	int getTreasureCount()
	{
		return treasureCount;
	}

	int getTrapCount()
	{
		return trapCount;
	}

	void switchViews()
	{
		TreasureButtonWidget->SetVisibility(ESlateVisibility::Hidden);
		TreasureButton->SetIsEnabled(false);
		TrapButtonWidget->SetVisibility(ESlateVisibility::Hidden);
		GoblinButton->SetIsEnabled(false);
		GoblinButtonWidget->SetVisibility(ESlateVisibility::Visible);
		GoblinButton->SetIsEnabled(true);
	}

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UARButtonWidget* TreasureButtonWidget;

	UButton* TreasureButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UARButtonWidget* TrapButtonWidget;

	UButton* TrapButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UARButtonWidget* GoblinButtonWidget;
	
	UButton* GoblinButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* dwarfHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* otherHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* confirmButton;

	UPROPERTY()
	//subject for host button clicked
	USubject* buttonClicked = nullptr;

	UPROPERTY()
	UUIObserver* UIObserverInstance;

	TMap<EEvent, UARButtonWidget*> buttonList;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideInTreasure;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideOutTreasure;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideInTrap;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideOutTrap;


	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideInGoblin;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideOutGoblin;

private:
	UFUNCTION()
	void OnTreasureButtonClicked()
	{
		if (IsValid(this) && treasureCount > 0)
		{
			OnButtonClicked(EEvent::TREASURE_BUTTON);
		}
	}
	UFUNCTION()
	void OnTrapButtonClicked()
	{
		if (IsValid(this) && trapCount > 0)
		{
			OnButtonClicked(EEvent::TRAP_BUTTON);
		}
	}
	UFUNCTION()
	void OnGoblinButtonClicked()
	{
		if (IsValid(this))
		{
			OnButtonClicked(EEvent::GOBLIN_BUTTON);
		}
	}
	UFUNCTION()
	void OnConfirmButtonClicked()
	{
		OnButtonClicked(EEvent::CONFIRM_BUTTON_MAIN);
	}

	void OnButtonClicked(EEvent event);

	EEvent currentlySelectedButtonType = EEvent::EMPTY;

	EEvent lastSelectedButtonType = EEvent::EMPTY;

	void moveButtons(EEvent event);

	int treasureCount = 5;

	int trapCount = 5;
};
