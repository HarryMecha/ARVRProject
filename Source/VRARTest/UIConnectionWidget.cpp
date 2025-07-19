// Fill out your copyright notice in the Description page of Project Settings.


#include "UIConnectionWidget.h"
#include "UIObserver.h"
#include "ARPawn.h"

void UUIConnectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (hostButton)
	{
		hostButton->OnClicked.AddDynamic(this, &UUIConnectionWidget::OnHostButtonClicked);
	}

	if (clientButton)
	{
		clientButton->OnClicked.AddDynamic(this, &UUIConnectionWidget::OnClientButtonClicked);
	}

	if (confirmButton)
	{
		confirmButton->OnClicked.AddDynamic(this, &UUIConnectionWidget::OnConfirmButtonClicked);
		changeConfirmButtonVisibility();
	}

}

void UUIConnectionWidget::OnConnectionButtonClicked(Event event)
{
	switch (event) {
	case(HOST_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Host Button Clicked"));
		connectionButtonClicked.notify(event);
		hostButton->SetVisibility(ESlateVisibility::Hidden);
		clientButton->SetVisibility(ESlateVisibility::Hidden);
		if (buttonsBorder) buttonsBorder->SetVisibility(ESlateVisibility::Hidden);
		break;
	case(CLIENT_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Client Button Clicked"));
		connectionButtonClicked.notify(event);
		hostButton->SetVisibility(ESlateVisibility::Hidden);
		clientButton->SetVisibility(ESlateVisibility::Hidden);
		if (buttonsBorder) buttonsBorder->SetVisibility(ESlateVisibility::Hidden);
		break;

	case(CONFIRM_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Clicked"));
		connectionButtonClicked.notify(event);
		changeConfirmButtonVisibility();
		break;
	}
}

Subject& UUIConnectionWidget::GetConnectionButtonSubject()
{
	return connectionButtonClicked;
}

void UUIConnectionWidget::changeConfirmButtonVisibility()
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


void  UUIConnectionWidget::setupUIObserver(AARPawn* arPawn = nullptr)
{
	UWorld* currentWorld = GetWorld();
	
	UIObserverInstance = NewObject<UUIObserver>(this);
	UIObserverInstance->init(currentWorld, arPawn);
	GetConnectionButtonSubject().addObserver(UIObserverInstance);
	
}