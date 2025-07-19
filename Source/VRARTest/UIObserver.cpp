#include "UIObserver.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UDPCommunicationsManager.h"
#include "UIConnectionWidget.h"
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


void UUIObserver::onNotify(Event event)
{
	switch (event)
	{
	case HOST_BUTTON:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Host Button Notified"));
		
		udpManager->setDeviceTypeSelected();
		udpManager->setHostDevice(true);
		if (arPawn)
		{
			arPawn->startARSession();
		}
		
		break;

	case CLIENT_BUTTON:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Client Button Notified"));
		udpManager->setDeviceTypeSelected();
		udpManager->setHostDevice(false);
		if (arPawn)
		{
			arPawn->startARSession();
		}
		break;
	
	case CONFIRM_BUTTON:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Confirm Button Notified"));
		arPawn->SpawnMap();
		break;
	}
}