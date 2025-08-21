// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/ARButtonWidget.h"
#include "Components/Border.h"
#include "UI/HealthBarWidget.h"
#include "UI/Subject.h"
class UUIObserver;
class AARPawn;
#include "ARMapSetupUI.generated.h"

UCLASS()
class VRARTEST_API UARMapSetupUI : public UUserWidget
{

	GENERATED_BODY()
public:
	virtual void NativeConstruct() override; //Just links up all the buttons to the their onClicked function calls.

	// changeButtonVisibility takes a button and boolean as inputs and sets the buttons visibility based on the boolean.
	void changeButtonVisibility(UButton* button, bool toggle);

	// setupUIObserver() function sets up functionality for the UIObserver to observe this specific class for button clicks.
	// A new instance of the obsever is created with the currentWorld and reference to the owning ARPawn being passed in, 
	// when this is successfully done the subject is set so that it can notify the observer when a button is clicked, with the 
	// buttonClicked->addObserver(UIObserverInstance); adding it.
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

	//switchViews() turns on and off specific button types both in functionality and in visibility going from the map setup stage to the regular ar player view.
	void switchViews();

	//zoomedInView() is called when the map is zoomed in or out it will turn off all the buttons apart from the zoom button or the opposite depending on the value of the boolean.
	void zoomedInView(bool toggle);
	
	//toggleAllButtons() sets the visibility and functionality of all the buttons depending on the boolean.
	void toggleAllButtons(bool toggle);

	// incrimentCooldownArray() just iterates through all the buttons currently on cooldown and checks whether their cooldown has expired, if it has they are removed
	// from the array and re-enabled.
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

	//setPopUpText() takes an FString as an input, converts it to a TEXT object and then sets the popUpText to that object.
	void setPopUpText(FString text);
	
	UTextBlock* getZoomButtonTitle()
	{
		return ZoomButtonTitle;
	}

protected:

	// Declerations of all the ButtonWidgets and Button Objects.

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

	//All the functions below contain similar logic all calling OnButtonClicked with their respective event types 

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

	//the main OnButtonClicked function, it takes the event type received and compares it using a switch case. If the zoomButton is clicked then the buttonClicked
	// subject will notify the observer to handle logic for this event. If any other button is clicked it simply sets the currentlySelectedButtonType to the event of
	// the button clicked. With the sole exception of CONFIRM_BUTTON_MAIN which will look at the specific Event type of currentlySelectedButtonType and perform logic
	// according to it. This logic will either be subtracting from chest and trap counts in the AR Players intial setup phase or adding certain button types to the 
	// cooldown array. The subject is then notified to perform the logic based on the currentlySelectedButtonType.
	void OnButtonClicked(EEvent event);

	EEvent currentlySelectedButtonType = EEvent::EMPTY;

	EEvent lastSelectedButtonType = EEvent::EMPTY;

	// moveButtons will check the currentlySelectedButtonType and move play that buttons SlideIn animation bringing into view and will play the lastSelectedButtonType's
	// SlideOut animation bringing in out of view.
	void moveButtons(EEvent event);

	int treasureCount = 5;

	int trapCount = 5;

	TArray<UARButtonWidget*> cooldownArray;

	bool setUpComplete = false;

};
