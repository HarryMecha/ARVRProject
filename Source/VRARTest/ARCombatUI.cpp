// Fill out your copyright notice in the Description page of Project Settings.


#include "ARCombatUI.h"
#include "UIObserver.h"

void UARCombatUI::NativeConstruct()
{
	if (!buttonClicked)
	{
		buttonClicked = NewObject<USubject>(this, USubject::StaticClass());
	}
	if (fireButton)
	{
		fireButton->OnClicked.AddDynamic(this, &UARCombatUI::OnFireButtonClicked);
	}
	if (dwarfHealthBar)
	{
		dwarfHealthBar->setHealthBarHeaderText("Dwarf Health:");
		dwarfHealthBar->CreateHealthBar(5);
	}
	if (wizardHealthBar)
	{
		wizardHealthBar->setHealthBarHeaderText("Wizard Health:");
	}
}

void UARCombatUI::OnButtonClicked(EEvent event)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Fire Button  has been Clicked"));

	if (GetWorld()->GetTimeSeconds() - timeSinceLastFire > fireCooldown)
	{
		if (event == EEvent::FIRE_BUTTON)
		{
			timeSinceLastFire = GetWorld()->GetTimeSeconds();
			buttonClicked->notify(event);
		}
	}
	
}

void UARCombatUI::setupUIObserver(AARCharacter* arCharacter = nullptr)
{
	UWorld* currentWorld = GetWorld();

	UIObserverInstance = NewObject<UUIObserver>(this);
	UIObserverInstance->init(currentWorld, nullptr, arCharacter);
	if (!buttonClicked)
	{
		buttonClicked = NewObject<USubject>(this);
		if (!buttonClicked)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to create connectionButtonClicked subject!"));
			return;
		}
	}

	if (!UIObserverInstance)
	{
		UIObserverInstance = NewObject<UUIObserver>(this);
		UIObserverInstance->init(currentWorld, nullptr, arCharacter);
	}

	buttonClicked->addObserver(UIObserverInstance);

}