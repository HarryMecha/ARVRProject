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


	buttonList.Add(EEvent::TREASURE_BUTTON, TreasureButtonWidget);
	buttonList.Add(EEvent::TRAP_BUTTON, TrapButtonWidget);

	if (TreasureButtonWidget)
	{
		TreasureButton = TreasureButtonWidget->getButton();
		TreasureButtonWidget->setNumberText(treasureCount);
		TreasureButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnTreasureButtonClicked);

	}

	if (TrapButtonWidget)
	{
		TrapButton = TrapButtonWidget->getButton();
		TrapButtonWidget->setNumberText(trapCount);
		TrapButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnTrapButtonClicked);
	}

	if (GoblinButtonWidget)
	{
		GoblinButton = GoblinButtonWidget->getButton();
		GoblinButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnGoblinButtonClicked);
		GoblinButtonWidget->SetVisibility(ESlateVisibility::Hidden);
		GoblinButton->SetIsEnabled(false);
	}

	if (confirmButton)
	{
		confirmButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnConfirmButtonClicked);
		changeButtonVisibility(confirmButton);
	}

	if (dwarfHealthBar)
	{
		dwarfHealthBar->setHealthBarHeaderText("Dwarf Health:");
		dwarfHealthBar->CreateHealthBar(5);
	}

	if (otherHealthBar)
	{
		otherHealthBar->setHealthBarHeaderText("??? Health:");
		otherHealthBar->CreateHealthBar(0);
		otherHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UARMapSetupUI::OnButtonClicked(EEvent event)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("A Button  has been Clicked"));

	switch (event) {
	case(EEvent::TREASURE_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Treasure Button Clicked"));
		if (currentlySelectedButtonType == EEvent::EMPTY || currentlySelectedButtonType != EEvent::TREASURE_BUTTON)
		{
			lastSelectedButtonType = currentlySelectedButtonType;
			currentlySelectedButtonType = EEvent::TREASURE_BUTTON;
			buttonClicked->notify(event);
			

		}

		break;
	case(EEvent::TRAP_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Trap Button Clicked"));
		if (currentlySelectedButtonType == EEvent::EMPTY || currentlySelectedButtonType != EEvent::TRAP_BUTTON)
		{
			lastSelectedButtonType = currentlySelectedButtonType;
			currentlySelectedButtonType = EEvent::TRAP_BUTTON;
			buttonClicked->notify(event);
		}
		break;
	
	case(EEvent::GOBLIN_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Goblin Button Clicked"));
		if (currentlySelectedButtonType == EEvent::EMPTY || currentlySelectedButtonType != EEvent::GOBLIN_BUTTON)
		{
			lastSelectedButtonType = currentlySelectedButtonType;
			currentlySelectedButtonType = EEvent::GOBLIN_BUTTON;
			buttonClicked->notify(event);
		}
		break;

	case(EEvent::CONFIRM_BUTTON_MAIN):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Clicked"));
		if (currentlySelectedButtonType == EEvent::TRAP_BUTTON)
		{
			trapCount--;
			TrapButtonWidget->setNumberText(trapCount);
		}
		if (currentlySelectedButtonType == EEvent::TREASURE_BUTTON)
		{
			treasureCount--;
			TreasureButtonWidget->setNumberText(treasureCount);
		}
		buttonClicked->notify(event);
		break;
	}
	moveButtons(currentlySelectedButtonType);
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

void UARMapSetupUI::moveButtons(EEvent event)
{
	if (currentlySelectedButtonType != lastSelectedButtonType)
	{
		if (lastSelectedButtonType == EEvent::TREASURE_BUTTON)
		{
			PlayAnimation(SlideOutTreasure);
		}
		else if (lastSelectedButtonType == EEvent::TRAP_BUTTON)
		{
			PlayAnimation(SlideOutTrap);
		}
		else if (lastSelectedButtonType == EEvent::GOBLIN_BUTTON)
		{
			PlayAnimation(SlideOutGoblin);

		}
		if (currentlySelectedButtonType == EEvent::TREASURE_BUTTON)
		{
			PlayAnimation(SlideInTreasure);
		}
		else if (currentlySelectedButtonType == EEvent::TRAP_BUTTON)
		{
			PlayAnimation(SlideInTrap);
		}
		else if (currentlySelectedButtonType == EEvent::GOBLIN_BUTTON)
		{
			PlayAnimation(SlideInGoblin);
		}


		lastSelectedButtonType = currentlySelectedButtonType;
	}
}