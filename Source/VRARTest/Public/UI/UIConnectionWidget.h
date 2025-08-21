// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Slider.h"
#include "UI/Observer.h"
#include "UI/Subject.h"
class UUIObserver;
class AARPawn;
#include "UIConnectionWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UUIConnectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//changeWidgetVisibility changes the visibility of the given widget item.
	void changeWidgetVisibility(UWidget* widgetItem);

	// setupUIObserver() function sets up functionality for the UIObserver to observe this specific class for button clicks.
	// A new instance of the obsever is created with the currentWorld and reference to the owning ARPawn being passed in, 
	// when this is successfully done the subject is set so that it can notify the observer when a button is clicked, with the 
	// buttonClicked->addObserver(UIObserverInstance); adding it.
	UFUNCTION(BlueprintCallable, Category = "UI")
	void setupUIObserver(AARPawn* pawn);

	USlider* getScaleSlider()
	{
		return scaleSlider;
	}

	UButton* getConfirmButton()
	{
		return confirmButton;
	}

	UButton* getLeftRotateButton()
	{
		return leftRotateButton;
	}

	UButton* getRightRotateButton()
	{
		return rightRotateButton;
	}

	UButton* getResetButton()
	{
		return resetButton;
	}

	//setPopUpText() takes an FString as an input, converts it to a TEXT object and then sets the popUpText to that object.
	void setPopUpText(FString text);

protected:
	virtual void NativeConstruct() override; // assigns button to their click events

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* hostButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* clientButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UButton* confirmButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UBorder* buttonsBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UButton* leftRotateButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UButton* rightRotateButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	USlider* scaleSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UButton* resetButton;

	
	UPROPERTY()
	USubject* connectionButtonClicked = nullptr;

	UPROPERTY()
	UUIObserver* UIObserverInstance;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* popUpText;

private:
	
	//All the functions below contain similar logic all calling OnButtonClicked with their respective event types 

	UFUNCTION()
	void OnHostButtonClicked()
	{
		OnConnectionButtonClicked(EEvent::HOST_BUTTON);
	}
	UFUNCTION()
	void OnClientButtonClicked()
	{
		OnConnectionButtonClicked(EEvent::CLIENT_BUTTON);
	}
	UFUNCTION()
	void OnConfirmButtonClicked()
	{
		OnConnectionButtonClicked(EEvent::CONFIRM_BUTTON);
	}
	
	//notifies the subject of a SLIDER_CHANGE event when the slider is moved.
	UFUNCTION()
	void OnScaleSliderValueChanged(float Value);


	// OnConnectionButtonClicked function, it takes the event type received and compares it using a switch case. It will look at the specific Event type 
	// and notify the subject of that specific event occuring.
	void OnConnectionButtonClicked(EEvent event);

	UFUNCTION()
	void OnLeftRotateButtonPressed()
	{
		connectionButtonClicked->notify(EEvent::LEFT_ROTATION,0,true);
	}

	UFUNCTION()
	void OnLeftRotateButtonReleased()
	{
		connectionButtonClicked->notify(EEvent::LEFT_ROTATION, 0, false);
	}

	UFUNCTION()
	void OnRightRotateButtonPressed()
	{
		connectionButtonClicked->notify(EEvent::RIGHT_ROTATION,0, true);

	}

	UFUNCTION()
	void OnRightRotateButtonReleased()
	{
		connectionButtonClicked->notify(EEvent::RIGHT_ROTATION, 0,false);
	}

	UFUNCTION()
	void OnResetButtonClicked()
	{
		connectionButtonClicked->notify(EEvent::RESET_BUTTON, 0, false);

	}

};
