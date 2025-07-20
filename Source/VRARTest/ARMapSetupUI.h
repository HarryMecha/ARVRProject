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

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* TreasureButtonObject;

	TPair<UButton*, EButtonState> treasureButton = MakeTuple(TreasureButtonObject, EButtonState::RECEEDED);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* TrapButtonObject;

	TPair<UButton*, EButtonState> trapButton = MakeTuple(TrapButtonObject, EButtonState::RECEEDED);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* confirmButton;

	//subject for host button clicked
	Subject* buttonClicked = new Subject();

	UUIObserver* UIObserverInstance;

	TMap<TPair<UButton*, EButtonState>, Event> buttonList;
private:
	UFUNCTION()
	void OnTreasureButtonClicked()
	{
		OnButtonClicked(TREASURE_BUTTON);
	}
	UFUNCTION()
	void OnTrapButtonClicked()
	{
		OnButtonClicked(TRAP_BUTTON);
	}
	UFUNCTION()
	void OnConfirmButtonClicked()
	{
		OnButtonClicked(CONFIRM_BUTTON_MAIN);
	}

	void OnButtonClicked(Event event);

	Event currentlySelectedButtonType = EMPTY;

	void moveButtons();
};
