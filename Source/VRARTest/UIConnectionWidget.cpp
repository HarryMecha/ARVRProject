// Fill out your copyright notice in the Description page of Project Settings.


#include "UIConnectionWidget.h"
#include "UIObserver.h"
#include "ARPawn.h"
#include "Subject.h"

void UUIConnectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!connectionButtonClicked)
	{
		connectionButtonClicked = NewObject<USubject>(this, USubject::StaticClass());
	}

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
		confirmButton->SetVisibility(ESlateVisibility::Hidden);
	}

	
	if (scaleSlider)
	{
		scaleSlider->OnValueChanged.AddDynamic(this, &UUIConnectionWidget::OnScaleSliderValueChanged);

		scaleSlider->SetMinValue(0.1f);
		scaleSlider->SetMaxValue(5.0f);
		scaleSlider->SetValue(1.0f);

		scaleSlider->SetVisibility(ESlateVisibility::Hidden);
	}

	if (leftRotateButton)
	{
		leftRotateButton->OnPressed.AddDynamic(this, &UUIConnectionWidget::OnLeftRotateButtonPressed);
		leftRotateButton->OnReleased.AddDynamic(this, &UUIConnectionWidget::OnLeftRotateButtonReleased);

		leftRotateButton->SetVisibility(ESlateVisibility::Hidden);
	}

	if (rightRotateButton)
	{
		rightRotateButton->OnPressed.AddDynamic(this, &UUIConnectionWidget::OnRightRotateButtonPressed);
		rightRotateButton->OnReleased.AddDynamic(this, &UUIConnectionWidget::OnRightRotateButtonReleased);

		rightRotateButton->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (resetButton)
	{
		resetButton->OnClicked.AddDynamic(this, &UUIConnectionWidget::OnResetButtonClicked);
		resetButton->SetVisibility(ESlateVisibility::Hidden);
	}
}


void UUIConnectionWidget::OnConnectionButtonClicked(EEvent event)
{
	switch (event) {
	case(EEvent::HOST_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Host Button Clicked"));
		connectionButtonClicked->notify(event);
		hostButton->SetVisibility(ESlateVisibility::Hidden);
		clientButton->SetVisibility(ESlateVisibility::Hidden);
		if (buttonsBorder) buttonsBorder->SetVisibility(ESlateVisibility::Hidden);
		break;
	case(EEvent::CLIENT_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Client Button Clicked"));
		connectionButtonClicked->notify(event);
		hostButton->SetVisibility(ESlateVisibility::Hidden);
		clientButton->SetVisibility(ESlateVisibility::Hidden);
		if (buttonsBorder) buttonsBorder->SetVisibility(ESlateVisibility::Hidden);
		break;

	case(EEvent::CONFIRM_BUTTON):
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Clicked"));
		connectionButtonClicked->notify(event);
		break;
	}
}


void UUIConnectionWidget::changeWidgetVisibility(UWidget* widgetItem)
{
	switch (widgetItem->GetVisibility()) {
	case(ESlateVisibility::Hidden):
		widgetItem->SetVisibility(ESlateVisibility::Visible);
		break;
	case(ESlateVisibility::Visible):
		widgetItem->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
}

void  UUIConnectionWidget::setupUIObserver(AARPawn* arPawn = nullptr)
{
	UWorld* currentWorld = GetWorld();
	
	UIObserverInstance = NewObject<UUIObserver>(this);
	UIObserverInstance->init(currentWorld, arPawn);
	if (!connectionButtonClicked)
	{
		connectionButtonClicked = NewObject<USubject>(this);
		if (!connectionButtonClicked)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create connectionButtonClicked subject!"));
			return;
		}
	}

	if (!UIObserverInstance)
	{
		UIObserverInstance = NewObject<UUIObserver>(this);
		UIObserverInstance->init(currentWorld, arPawn);
	}

	connectionButtonClicked->addObserver(UIObserverInstance);
	
}

void UUIConnectionWidget::OnScaleSliderValueChanged(float value)
{

	connectionButtonClicked->notify(EEvent::SLIDER_CHANGE, value, false);
	
}
