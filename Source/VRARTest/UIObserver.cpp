#include "UIObserver.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UDPCommunicationsManager.h"
#include "UIConnectionWidget.h"
#include "ARMapSetupUI.h"
#include "ARPawn.h"
#include <Kismet/GameplayStatics.h>


UUIObserver::UUIObserver()
{
	udpManager = nullptr;
	arPawn = nullptr;
}

void UUIObserver::init(UWorld* world, AARPawn* pawn)
{
	
	for (TActorIterator<AUDPCommunicationsManager> It(world); It; ++It)
	{
		udpManager = *It;
		break;
	}
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if(pawn)
	arPawn = pawn;
}


void UUIObserver::OnNotify_Implementation(EEvent event, float value, bool b)
{
	switch (event)
	{
	case EEvent::HOST_BUTTON:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Host Button Notified"));

		udpManager->setDeviceTypeSelected();
		udpManager->setHostDevice(true);
		break;

	case EEvent::CLIENT_BUTTON:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Client Button Notified"));
		udpManager->setDeviceTypeSelected();
		udpManager->setHostDevice(false);

		break;

	case EEvent::CONFIRM_BUTTON:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Notified"));
		if (!arPawn->getMapSpawned()) 
		{
			arPawn->SpawnMap();
		}
		else
		{
			arPawn->ConfirmMapChoice();
		}
		break;

	case EEvent::SLIDER_CHANGE:
		if (arPawn)
		{
			arPawn->SetMapScale(value);
		}
		break;

	case EEvent::TREASURE_BUTTON:
		if (arPawn) {
			arPawn->setInteractionMode(EInteractionMode::SpawningObject, ESpawnableObject::Chest);
		}
		break;

	case EEvent::TRAP_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::SpawningObject, ESpawnableObject::Trap);
		}
		break;

	case EEvent::GOBLIN_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::SpawningObject, ESpawnableObject::Goblin);
		}
		break;
	
	case EEvent::BLOCK_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::BlockingTunnel);
		}
		break;

	case EEvent::SWAP_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::SwappingObjects);
		}
		break;

	case EEvent::FRENZY_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::ApplyingFrenzy);
		}
		break;

	case EEvent::CONFIRM_BUTTON_MAIN:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Notified"));
		if (arPawn->getBlockEnabled() == true)
		{
			arPawn->blockTunnel();
			arPawn->resetSelection();
			arPawn->getMapSetupWidget()->changeButtonVisibility(arPawn->getMapSetupWidget()->getConfirmButton(),false);
		}
		else if (arPawn->getSwapEnabled() == true)
		{
			arPawn->swapObjects();
			arPawn->resetSelection();
			arPawn->getMapSetupWidget()->changeButtonVisibility(arPawn->getMapSetupWidget()->getConfirmButton(), false);
		}
		else if (arPawn->getFrenzyEnabled() == true)
		{
			arPawn->applyFrenzy();
			arPawn->resetSelection();
			arPawn->getMapSetupWidget()->changeButtonVisibility(arPawn->getMapSetupWidget()->getConfirmButton(), false);
		}
		else
		{
			arPawn->spawnObject();
			arPawn->resetSelection();
			arPawn->getMapSetupWidget()->changeButtonVisibility(arPawn->getMapSetupWidget()->getConfirmButton(), false);
		}

		break;

	case EEvent::LEFT_ROTATION:
		if (arPawn)
		{
			arPawn->toggleMapLeftRotate(b);
		}
		break;
	case EEvent::RIGHT_ROTATION:
		if (arPawn)
		{
			arPawn->toggleMapRightRotate(b);
		}
		break;
	case EEvent::RESET_BUTTON:
		if (arPawn)
		{
			arPawn->resetARState();
		}
		break;
	case EEvent::EMPTY:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::None);
		}
		break;
	}
}