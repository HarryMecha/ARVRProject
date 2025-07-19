// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
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
	Subject& GetConnectionButtonSubject();

	void changeConfirmButtonVisibility();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void setupUIObserver(AARPawn* pawn);

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

	//subject for host button clicked
	Subject connectionButtonClicked;

	UUIObserver* UIObserverInstance;

private:
	UFUNCTION()
	void OnHostButtonClicked()
	{
		OnConnectionButtonClicked(HOST_BUTTON);
	}
	UFUNCTION()
	void OnClientButtonClicked()
	{
		OnConnectionButtonClicked(CLIENT_BUTTON);
	}
	UFUNCTION()
	void OnConfirmButtonClicked()
	{
		OnConnectionButtonClicked(CONFIRM_BUTTON);
	}

	void OnConnectionButtonClicked(Event event);

};
