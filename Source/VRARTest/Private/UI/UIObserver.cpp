#include "UI/UIObserver.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Networking/UDPCommunicationsManager.h"
#include "UI/UIConnectionWidget.h"
#include "UI/ARMapSetupUI.h"
#include "ARSpecific/ARPawn.h"
#include "ARSpecific/ARCharacter.h"
#include <Kismet/GameplayStatics.h>


UUIObserver::UUIObserver()
{
	udpManager = nullptr;
	arPawn = nullptr;
}

void UUIObserver::init(UWorld* world, AARPawn* pawn, AARCharacter* character)
{
	
	for (TActorIterator<AUDPCommunicationsManager> It(world); It; ++It)
	{
		udpManager = *It;
		break;
	}
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (pawn)
	{
		arPawn = pawn;
	}

	if (character)
	{
		arCharacter = character;
	}
}


void UUIObserver::OnNotify_Implementation(EEvent event, float value, bool b)
{
	switch (event)
	{
	//Case for the host button being clicked this will set the selected device type in the udpManager.
	case EEvent::HOST_BUTTON:
		udpManager->setDeviceTypeSelected();
		udpManager->setHostDevice(true);
		break;
	//Case for the client button being clicked this will set the selected device type in the udpManager.
	case EEvent::CLIENT_BUTTON:
		udpManager->setDeviceTypeSelected();
		udpManager->setHostDevice(false);

		break;
	//Case for the confirm button only handles confirmation of map choices either confirming the map's rotation and position
	// have been set by the player sufficiently or the plane on which the map will be spawned has been confirmed.
	case EEvent::CONFIRM_BUTTON:
		if (!arPawn->getMapSpawned()) 
		{
			arPawn->SpawnMap();
		}
		else
		{
			arPawn->ConfirmMapChoice();
		}
		break;
	//Case for changes in the slider value that control the map scale during setup.
	case EEvent::SLIDER_CHANGE:
		if (arPawn)
		{
			arPawn->SetMapScale(value);
		}
		break;
	//Case for the treasure button being clicked this will set the interaction mode in the arPawn to spawningObject and the object to spawn being a chest.
	case EEvent::TREASURE_BUTTON:
		if (arPawn) {
			arPawn->setInteractionMode(EInteractionMode::SpawningObject, ESpawnableObject::Chest);
		}
		break;
	//Case for the trap button being clicked this will set the interaction mode in the arPawn to spawningObject and the object to spawn being a trap.
	case EEvent::TRAP_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::SpawningObject, ESpawnableObject::Trap);
		}
		break;
	//Case for the goblin button being clicked this will set the interaction mode in the arPawn to spawningObject and the object to spawn being a goblin.
	case EEvent::GOBLIN_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::SpawningObject, ESpawnableObject::Goblin);
		}
		break;
	//Case for the block button being clicked this will set the interaction mode in the arPawn to BlockingTunnel.
	case EEvent::BLOCK_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::BlockingTunnel);
		}
		break;
	//Case for the swap button being clicked this will set the interaction mode in the arPawn to SwappingObjects.
	case EEvent::SWAP_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::SwappingObjects);
		}
		break;
	//Case for the frenzy button being clicked this will set the interaction mode in the arPawn to ApplyingFrenzy.
	case EEvent::FRENZY_BUTTON:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::ApplyingFrenzy);
		}
		break;
	//Case for the zoom button being clicked this will set the interaction mode in the arPawn to ZoomingIn.
	case EEvent::ZOOM_BUTTON:
    if (arPawn)
    {
        arPawn->setInteractionMode(EInteractionMode::ZoomingIn);
    }
		break;
	
	//Case for the main confirm button on the main AR UI clicking this will have a number of effects based on the field that are true,
	// if the block is enable it will blovk off the tunnel selected, if swap is enabled it will call for the teo objects at the selected 
	// sections to be swapped, if frenzy is enabled it will apply frenzy to the entity in that specific map section, or that would mean
	// spawning object was true and will spawn an object of a specified type at the selected map section.
	case EEvent::CONFIRM_BUTTON_MAIN:
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
	//Case for clicking the left rotation button that control the rotation of the map during setup.
	case EEvent::LEFT_ROTATION:
		if (arPawn)
		{
			arPawn->toggleMapLeftRotate(b);
		}
		break;
	//Case for clicking the right rotation button that control the rotation of the map during setup.
	case EEvent::RIGHT_ROTATION:
		if (arPawn)
		{
			arPawn->toggleMapRightRotate(b);
		}
		break;
	//Case for clicking the reset button forcing the destruction of all currently created planes in order to regenerate new one.
	case EEvent::RESET_BUTTON:
		if (arPawn)
		{
			arPawn->resetARState();
		}
		break;
	//Case for clicking the fire button by the AR Playing spawning a spell entity at the spawn point.
	case EEvent::FIRE_BUTTON:
		if (arCharacter)
		{
			arCharacter->spawnSpell();
		}
		break;
	//Case for empty that just resets the interaction mode back to none.
	case EEvent::EMPTY:
		if (arPawn)
		{
			arPawn->setInteractionMode(EInteractionMode::None);
		}
		break;
	}
}