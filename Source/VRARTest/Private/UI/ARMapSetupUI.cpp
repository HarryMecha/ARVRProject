// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ARMapSetupUI.h"
#include "UI/UIObserver.h"
#include "ARSpecific/ARPawn.h"

void UARMapSetupUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (!buttonClicked)
	{
		buttonClicked = NewObject<USubject>(this, USubject::StaticClass());
	}

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

	if (BlockButtonWidget)
	{
		BlockButton = BlockButtonWidget->getButton();
		BlockButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnBlockButtonClicked);
		BlockButtonWidget->SetVisibility(ESlateVisibility::Hidden);
		BlockButton->SetIsEnabled(false);
		if (BlockButtonWidget->getTimerIcon())
		{
			BlockButtonWidget->getTimerIcon()->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (SwapButtonWidget)
	{
		SwapButton = SwapButtonWidget->getButton();
		SwapButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnSwapButtonClicked);
		SwapButtonWidget->SetVisibility(ESlateVisibility::Hidden);
		SwapButton->SetIsEnabled(false);
		if (SwapButtonWidget->getTimerIcon())
		{
			SwapButtonWidget->getTimerIcon()->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (FrenzyButtonWidget)
	{
		FrenzyButton = FrenzyButtonWidget->getButton();
		FrenzyButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnFrenzyButtonClicked);
		FrenzyButtonWidget->SetVisibility(ESlateVisibility::Hidden);
		FrenzyButton->SetIsEnabled(false);
	}

	if (zoomButton)
	{
		zoomButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnZoomButtonClicked);
		changeButtonVisibility(zoomButton, false);

	}

	if (confirmButton)
	{
		confirmButton->OnClicked.AddDynamic(this, &UARMapSetupUI::OnConfirmButtonClicked);
		changeButtonVisibility(confirmButton, false);
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
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("A Button  has been Clicked"));

	if (event == EEvent::ZOOM_BUTTON)
	{
		lastSelectedButtonType = currentlySelectedButtonType;
		currentlySelectedButtonType = event;
		buttonClicked->notify(event);
	}
	else if (event == EEvent::CONFIRM_BUTTON_MAIN)
	{
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
		if (currentlySelectedButtonType == EEvent::BLOCK_BUTTON)
		{
			BlockButtonWidget->currentCoolDownAmount = BlockButtonWidget->maxCoolDown;
			BlockButtonWidget->TimerIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			cooldownArray.Add(BlockButtonWidget);
		}
		if (currentlySelectedButtonType == EEvent::SWAP_BUTTON)
		{
			SwapButtonWidget->currentCoolDownAmount = SwapButtonWidget->maxCoolDown;
			SwapButtonWidget->TimerIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			cooldownArray.Add(SwapButtonWidget);
		}
		buttonClicked->notify(event);
		currentlySelectedButtonType = EEvent::EMPTY;
		incrimentCooldownArray();
	}
	else if (currentlySelectedButtonType != event)
	{
		lastSelectedButtonType = currentlySelectedButtonType;
		currentlySelectedButtonType = event;
		buttonClicked->notify(event);
	}
	else
	{
		lastSelectedButtonType = currentlySelectedButtonType;
		currentlySelectedButtonType = EEvent::EMPTY;
		buttonClicked->notify(EEvent::EMPTY);
	}

	moveButtons(currentlySelectedButtonType);
}

void UARMapSetupUI::changeButtonVisibility(UButton* button, bool toggle)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Button Visibility Change"));

	if (toggle == true)
	{
		button->SetVisibility(ESlateVisibility::Visible);
	}
	else if (toggle == false)
	{
		button->SetVisibility(ESlateVisibility::Hidden);
	}
}


void UARMapSetupUI::setupUIObserver(AARPawn* arPawn = nullptr)
{
	UWorld* currentWorld = GetWorld();

	UIObserverInstance = NewObject<UUIObserver>(this);
	UIObserverInstance->init(currentWorld, arPawn, nullptr);
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
		UIObserverInstance->init(currentWorld, arPawn, nullptr);
	}

	buttonClicked->addObserver(UIObserverInstance);

}

void UARMapSetupUI::moveButtons(EEvent event)
{
	if (currentlySelectedButtonType != lastSelectedButtonType)
	{
		switch (lastSelectedButtonType)
		{
		case(EEvent::TREASURE_BUTTON):
			PlayAnimation(SlideOutTreasure);
			break;
		case(EEvent::TRAP_BUTTON):
			PlayAnimation(SlideOutTrap);
			break;
		case(EEvent::GOBLIN_BUTTON):
			PlayAnimation(SlideOutGoblin);
			break;
		case(EEvent::BLOCK_BUTTON):
			PlayAnimation(SlideOutBlock);
			break;
		case(EEvent::SWAP_BUTTON):
			PlayAnimation(SlideOutSwap);
			break;
		case(EEvent::FRENZY_BUTTON):
			PlayAnimation(SlideOutFrenzy);
			break;
		}

		switch (currentlySelectedButtonType)
		{
		case(EEvent::TREASURE_BUTTON):
			PlayAnimation(SlideInTreasure);
			break;
		case(EEvent::TRAP_BUTTON):
			PlayAnimation(SlideInTrap);
			break;
		case(EEvent::GOBLIN_BUTTON):
			PlayAnimation(SlideInGoblin);
			break;
		case(EEvent::BLOCK_BUTTON):
			PlayAnimation(SlideInBlock);
			break;
		case(EEvent::SWAP_BUTTON):
			PlayAnimation(SlideInSwap);
			break;
		case(EEvent::FRENZY_BUTTON):
			PlayAnimation(SlideInFrenzy);
			break;
		}

		lastSelectedButtonType = currentlySelectedButtonType;
	}
}

void UARMapSetupUI::switchViews()
{
	TreasureButtonWidget->SetVisibility(ESlateVisibility::Hidden);
	TreasureButton->SetIsEnabled(false);
	TrapButtonWidget->SetVisibility(ESlateVisibility::Hidden);
	TrapButton->SetIsEnabled(false);
	GoblinButtonWidget->SetVisibility(ESlateVisibility::Visible);
	GoblinButton->SetIsEnabled(false);
	BlockButtonWidget->SetVisibility(ESlateVisibility::Visible);
	BlockButton->SetIsEnabled(false);
	SwapButtonWidget->SetVisibility(ESlateVisibility::Visible);
	SwapButton->SetIsEnabled(false);
	FrenzyButtonWidget->SetVisibility(ESlateVisibility::Visible);
	FrenzyButton->SetIsEnabled(false);
	setUpComplete = true;
}

void UARMapSetupUI::toggleAllButtons(bool toggle)
{
	GoblinButton->SetIsEnabled(toggle);
	BlockButton->SetIsEnabled(toggle);
	SwapButton->SetIsEnabled(toggle);
	FrenzyButton->SetIsEnabled(toggle);
}

void UARMapSetupUI::zoomedInView(bool toggle)
{
	if (setUpComplete == true)
	{
		if (toggle == true)
		{
			GoblinButtonWidget->SetVisibility(ESlateVisibility::Hidden);
			GoblinButton->SetIsEnabled(false);
			BlockButtonWidget->SetVisibility(ESlateVisibility::Hidden);
			BlockButton->SetIsEnabled(false);
			SwapButtonWidget->SetVisibility(ESlateVisibility::Hidden);
			SwapButton->SetIsEnabled(false);
			FrenzyButtonWidget->SetVisibility(ESlateVisibility::Hidden);
			FrenzyButton->SetIsEnabled(false);
		}
		else
		{
			GoblinButtonWidget->SetVisibility(ESlateVisibility::Visible);
			GoblinButton->SetIsEnabled(true);
			BlockButtonWidget->SetVisibility(ESlateVisibility::Visible);
			BlockButton->SetIsEnabled(true);
			SwapButtonWidget->SetVisibility(ESlateVisibility::Visible);
			SwapButton->SetIsEnabled(true);
			FrenzyButtonWidget->SetVisibility(ESlateVisibility::Visible);
			FrenzyButton->SetIsEnabled(true);
		}
	}
	else
	{
		if (toggle == true)
		{
			TreasureButtonWidget->SetVisibility(ESlateVisibility::Hidden);
			TreasureButton->SetIsEnabled(false);
			TrapButtonWidget->SetVisibility(ESlateVisibility::Hidden);
			TrapButton->SetIsEnabled(false);
		}
		else
		{
			TreasureButtonWidget->SetVisibility(ESlateVisibility::Visible);
			TreasureButton->SetIsEnabled(true);
			TrapButtonWidget->SetVisibility(ESlateVisibility::Visible);
			TrapButton->SetIsEnabled(true);
		}
	}
}

void UARMapSetupUI::setPopUpText(FString text)
{
	popUpText->SetText(FText::FromString(text));
}
