// Fill out your copyright notice in the Description page of Project Settings.


#include "ARMapSetupUI.h"
#include "UIObserver.h"
#include "ARPawn.h"

void UARMapSetupUI::NativeConstruct()
{
	Super::NativeConstruct();

	buttonList.Add(treasureButton,TREASURE_BUTTON);
	buttonList.Add(trapButton,TRAP_BUTTON);


	if (TreasureButtonObject)
	{
		TreasureButtonObject->OnClicked.AddDynamic(this, &UARMapSetupUI::OnTreasureButtonClicked);
	}

	if (TrapButtonObject)
	{
		TrapButtonObject->OnClicked.AddDynamic(this, &UARMapSetupUI::OnTrapButtonClicked);
	}

	if (confirmButton)
	{
		confirmButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnConfirmButtonClicked);
		changeButtonVisibility(confirmButton);
	}


}

void UARMapSetupUI::OnButtonClicked(Event event)
{
	switch (event) {
	case(TREASURE_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Treasure Button Clicked"));
		if (currentlySelectedButtonType ==  EMPTY || currentlySelectedButtonType != TREASURE_BUTTON)
		{
			currentlySelectedButtonType = TREASURE_BUTTON;
			buttonClicked->notify(event);

		}

		break;
	case(TRAP_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Trap Button Clicked"));
		if (currentlySelectedButtonType == EMPTY || currentlySelectedButtonType != TRAP_BUTTON)
		{
			currentlySelectedButtonType = TRAP_BUTTON;
			buttonClicked->notify(event);
		}
		break;

	case(CONFIRM_BUTTON_MAIN):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Clicked"));
		buttonClicked->notify(event);
		changeButtonVisibility(confirmButton);
		break;
	}
	moveButtons();
}

void UARMapSetupUI::moveButtons()
{
	for (auto entry : buttonList)
	{
		if (entry.Value == currentlySelectedButtonType)
		{
			//entry.Key.Key->SetBackgroundColor(FLinearColor::Blue);
			entry.Key.Value = EButtonState::EXTENDED;
		}
		else
		{
			//entry.Key.Key->SetBackgroundColor(FLinearColor::White);
			entry.Key.Value = EButtonState::RECEEDED;
		}

	}
}


void UARMapSetupUI::changeButtonVisibility(UButton* button)
{
	switch (button->GetVisibility()) {
	case(ESlateVisibility::Hidden):
		button->SetVisibility(ESlateVisibility::Visible);
		break;
	case(ESlateVisibility::Visible):
		button->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
}


void UARMapSetupUI::setupUIObserver(AARPawn* arPawn = nullptr)
{
	UWorld* currentWorld = GetWorld();

	UIObserverInstance = NewObject<UUIObserver>(this);
	UIObserverInstance->init(currentWorld, arPawn);
	buttonClicked->addObserver(UIObserverInstance);

}