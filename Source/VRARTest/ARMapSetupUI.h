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
#include "ARMapSetupUI.generated.h"

UENUM(BlueprintType)
enum class EButtonState : uint8
{
	EXTENDED,
	RECEEDED
};

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

	}

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* TreasureButton;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* TrapButton;

	TMap<UButton*, EButtonState> buttonStates;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* confirmButton;

	UPROPERTY()
	//subject for host button clicked
	USubject* buttonClicked = nullptr;

	UPROPERTY()
	UUIObserver* UIObserverInstance;

	TMap<UButton*, EEvent> buttonList;
private:
	UFUNCTION()
	void OnTreasureButtonClicked()
	{
		if (IsValid(this))
		{
			OnButtonClicked(EEvent::TREASURE_BUTTON);
		}
	}
	UFUNCTION()
	void OnTrapButtonClicked()
	{
		if (IsValid(this))
		{
			OnButtonClicked(EEvent::TRAP_BUTTON);
		}
	}
	UFUNCTION()
	void OnConfirmButtonClicked()
	{
		OnButtonClicked(EEvent::CONFIRM_BUTTON_MAIN);
	}

	void OnButtonClicked(EEvent event);

	EEvent currentlySelectedButtonType = EEvent::EMPTY;

	void moveButtons();
};
