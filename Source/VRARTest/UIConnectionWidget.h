// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Slider.h"
#include "Observer.h"
#include "Subject.h"
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
	void changeWidgetVisibility(UWidget* widgetItem);


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

	void setPopUpText(FString text);

protected:
	virtual void NativeConstruct() override;

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
	
	UFUNCTION()
	void OnScaleSliderValueChanged(float Value);

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
