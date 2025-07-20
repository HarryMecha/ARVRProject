#include "UIObserver.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UDPCommunicationsManager.h"
#include "UIConnectionWidget.h"
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
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("TREASURE_BUTTON case triggered"));
		if (arPawn) {
			arPawn->setObjectToSpawn(ESpawnableObject::Chest);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Hit Actor: %d"), StaticCast<uint8>(arPawn->getObjectToSpawn())));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ARPawn not found")));

		}
		break;

	case EEvent::TRAP_BUTTON:
		arPawn->setObjectToSpawn(ESpawnableObject::Trap);
		break;

	case EEvent::CONFIRM_BUTTON_MAIN:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Notified"));
		arPawn->spawnObject();
		arPawn->resetSelection();
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
	}
}