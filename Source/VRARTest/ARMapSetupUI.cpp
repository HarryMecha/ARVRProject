// Fill out your copyright notice in the Description page of Project Settings.


#include "ARMapSetupUI.h"
#include "ARPawn.h"

void UARMapSetupUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (treasureButton)
	{
		treasureButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnHostButtonClicked);
	}

	if (trapButton)
	{
		trapButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnClientButtonClicked);
	}

	if (confirmButton)
	{
		confirmButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnConfirmButtonClicked);
		changeConfirmButtonVisibility();
	}

}

void UARMapSetupUI::changeConfirmButtonVisibility()
{
	switch (confirmButton->GetVisibility()) {
	case(ESlateVisibility::Hidden):
		confirmButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case(ESlateVisibility::Visible):
		confirmButton->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
}

void UARMapSetupUI::setupUIObserver(AARPawn* arPawn = nullptr)
{
	UWorld* currentWorld = GetWorld();

	UIObserverInstance = NewObject<UUIObserver>(this, UIObserverClass);
	UIObserverInstance->init(currentWorld, arPawn);
	GetConnectionButtonSubject().addObserver(UIObserverInstance);

}