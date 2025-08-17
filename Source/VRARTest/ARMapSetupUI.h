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

	void changeButtonVisibility(UButton* button, bool toggle);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void setupUIObserver(AARPawn* pawn);

	UButton* getConfirmButton()
	{
		return confirmButton;
	}

	UButton* getZoomButton()
	{
		return zoomButton;
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

	void switchViews();

	void zoomedInView(bool toggle);

	void toggleAllButtons(bool toggle);

	void incrimentCooldownArray()
	{
		for (int32 i = cooldownArray.Num() - 1; i >= 0; --i)
		{
			// Get the widget at the current index
			UARButtonWidget* widget = cooldownArray[i];
			widget->currentCoolDownAmount--;
			if (widget->currentCoolDownAmount < 0)
			{
				widget->TimerIcon->SetVisibility(ESlateVisibility::Hidden);
				cooldownArray.RemoveAt(i);

			}
		}
	}

	void setPopUpText(FString text);
	
	UTextBlock* getZoomButtonTitle()
	{
		return ZoomButtonTitle;
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
	UARButtonWidget* BlockButtonWidget;

	UButton* BlockButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UARButtonWidget* SwapButtonWidget;

	UButton* SwapButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UARButtonWidget* FrenzyButtonWidget;

	UButton* FrenzyButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* dwarfHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* otherHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* confirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* zoomButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ZoomButtonTitle;

	UPROPERTY()
	USubject* buttonClicked = nullptr;

	UPROPERTY()
	UUIObserver* UIObserverInstance;

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

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideInBlock;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideOutBlock;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideInSwap;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideOutSwap;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideInFrenzy;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SlideOutFrenzy;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* popUpText;

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
	void OnBlockButtonClicked()
	{
		if (IsValid(this))
		{
			if (!cooldownArray.Contains(BlockButtonWidget))
			{
				OnButtonClicked(EEvent::BLOCK_BUTTON);
			}
		}
	}

	UFUNCTION()
	void OnSwapButtonClicked()
	{
		if (IsValid(this))
		{
			if (!cooldownArray.Contains(SwapButtonWidget))
			{
				OnButtonClicked(EEvent::SWAP_BUTTON);
			}
		}
	}

	UFUNCTION()
	void OnFrenzyButtonClicked()
	{
		if (IsValid(this))
		{
			if (!cooldownArray.Contains(FrenzyButtonWidget))
			{
				OnButtonClicked(EEvent::FRENZY_BUTTON);
			}
		}
	}

	UFUNCTION()
	void OnZoomButtonClicked()
	{
		OnButtonClicked(EEvent::ZOOM_BUTTON);
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

	int treasureCount = 1;

	int trapCount = 1;

	TArray<UARButtonWidget*> cooldownArray;

	bool setUpComplete = false;

};
