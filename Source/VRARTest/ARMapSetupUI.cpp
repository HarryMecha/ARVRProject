// Fill out your copyright notice in the Description page of Project Settings.


#include "ARMapSetupUI.h"
#include "UIObserver.h"
#include "ARPawn.h"

void UARMapSetupUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (!buttonClicked)
	{
		buttonClicked = NewObject<USubject>(this, USubject::StaticClass());
	}


	buttonList.Add(TreasureButton, EEvent::TREASURE_BUTTON);
	buttonList.Add(TrapButton, EEvent::TRAP_BUTTON);

	buttonStates.Add(TreasureButton, EButtonState::RECEEDED);
	buttonStates.Add(TrapButton, EButtonState::RECEEDED);

	if (TreasureButton)
	{
		TreasureButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnTreasureButtonClicked);
	}

	if (TrapButton)
	{
		TrapButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnTrapButtonClicked);
	}

	if (confirmButton)
	{
		confirmButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnConfirmButtonClicked);
		changeButtonVisibility(confirmButton);
	}


}

void UARMapSetupUI::OnButtonClicked(EEvent event)
{
	switch (event) {
	case(EEvent::TREASURE_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Treasure Button Clicked"));
		if (currentlySelectedButtonType == EEvent::EMPTY || currentlySelectedButtonType != EEvent::TREASURE_BUTTON)
		{
			currentlySelectedButtonType = EEvent::TREASURE_BUTTON;
			if (buttonClicked)
			{
				buttonClicked->notify(event);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Button Clicked is nullptr"));
			}

		}

		break;
	case(EEvent::TRAP_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Trap Button Clicked"));
		if (currentlySelectedButtonType == EEvent::EMPTY || currentlySelectedButtonType != EEvent::TRAP_BUTTON)
		{
			currentlySelectedButtonType = EEvent::TRAP_BUTTON;
			buttonClicked->notify(event);
		}
		break;

	case(EEvent::CONFIRM_BUTTON_MAIN):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Clicked"));
		buttonClicked->notify(event);
		break;
	}
	moveButtons();
}


void UARMapSetupUI::moveButtons()
{
	for (const auto& entry : buttonList)
	{
		UButton* button = entry.Key;
		EEvent mappedEvent = entry.Value;

		if (mappedEvent == currentlySelectedButtonType)
		{
			buttonStates[button] = EButtonState::EXTENDED;
		}
		else
		{
			buttonStates[button] = EButtonState::RECEEDED;
		}
	}
}


void UARMapSetupUI::changeButtonVisibility(UButton* button)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Button Visibility Change"));

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
	if (!buttonClicked)
	{
		buttonClicked = NewObject<USubject>(this);
		if (!buttonClicked)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create buttonClicked subject!"));
			return;
		}
	}

	if (!UIObserverInstance)
	{
		UIObserverInstance = NewObject<UUIObserver>(this);
		UIObserverInstance->init(currentWorld, arPawn);
	}

	buttonClicked->addObserver(UIObserverInstance);

}