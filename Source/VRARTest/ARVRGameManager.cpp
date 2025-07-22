// Fill out your copyright notice in the Description page of Project Settings.


#include "ARVRGameManager.h"
#include "Command.h"
#include "VRCharacter.h"
#include "ARPawn.h"
#include "Kismet/GameplayStatics.h"
#include "UDPCommunicationsManager.h"
#include "MapSection.h"
#include "GoblinPooledEntity.h"
#include "ChestPooledEntity.h"
#include "MapTunnel.h"
// Sets default values
AARVRGameManager::AARVRGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	arPawn = nullptr;
	vrCharacter = nullptr;
	udpCommunicationsManager = nullptr;
	vrRepresenation = nullptr;
	currentlyOccupiedSection = nullptr;
	currentTurn = EPlayerRole::AR;
}

// Called when the game starts or when spawned
void AARVRGameManager::BeginPlay()
{
	Super::BeginPlay();


	AActor* udpManagerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AUDPCommunicationsManager::StaticClass());
	udpCommunicationsManager = Cast<AUDPCommunicationsManager>(udpManagerActor);

	if (!udpCommunicationsManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find UDP Communications Manager!"));
	}

	if(LocalRole == EPlayerRole::AR)
	{
		APawn* FoundPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
		arPawn = Cast<AARPawn>(FoundPawn);
		if (!arPawn)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find ARPawn"));

		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		for (AActor* actor : AllActors) 
		{
			if (actor->Tags.Contains("MapRoot"))
			{
				mapRoot = actor;
			}
			if (actor->Tags.Contains("ARHide")) 
			{
				actor->SetActorHiddenInGame(true);
			}
			if (actor->Tags.Contains("MapSection"))
			{
				AMapSection* mapSection = Cast<AMapSection>(actor);
				mapSection->setManager(this);
				mapSection->toggleFog(false);
				mapSections.Add(mapSection);
			}
			if (vrRepresenationClass && actor->IsA(vrRepresenationClass))
			{
				vrRepresenation = Cast<AVRRepresentative>(actor);
			}
		}
	}
	if (LocalRole == EPlayerRole::VR)
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AVRCharacter::StaticClass());
		vrCharacter = Cast<AVRCharacter>(FoundActor);
		if (!vrCharacter)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find VRCharacter"));

		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
		for (AActor* actor : AllActors)
		{
			if (actor->Tags.Contains("VRHide"))
			{
				actor->SetActorHiddenInGame(true);
				actor->SetActorEnableCollision(false);
			}
			if (actor->Tags.Contains("MapSection"))
			{
				AMapSection* mapSection = Cast<AMapSection>(actor);
				mapSection->setManager(this);
				mapSection->toggleFog(true);
				mapSections.Add(mapSection);
			}
		}

	}
	sortMapSections();
	currentlyOccupiedSection = mapSections[0];
	setupPools();
}

// Called every frame
void AARVRGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ManageCommandQueue();

}

void AARVRGameManager::ManageCommandQueue()
{
	while (!outgoingCommandQueue.IsEmpty() && udpCommunicationsManager->getConnectionEstablished()) {
		TSharedPtr<Command> command;
		if (outgoingCommandQueue.Dequeue(command)) {
			udpCommunicationsManager->SendMessage(command->serialise(this));
		}
	}

	while (!incomingCommandQueue.IsEmpty()) {
		TSharedPtr<Command> command;
		if (incomingCommandQueue.Dequeue(command)) {
			command->execute(this);
		}
	}
}

//Receiving Messages
void AARVRGameManager::AddToIncomingCommandQueue(TArray<uint8> receivedPacket)
{
	uint8 messageTypeByte = receivedPacket[0];
	EMessageType messageType = static_cast<EMessageType>(messageTypeByte);

	switch (messageType) {
		case(EMessageType::Connection):
		{
			if (udpCommunicationsManager->getConnectionEstablished() == false)
			{
				TArray<uint8> packet;
				packet.Add(static_cast<uint8>(EMessageType::Connection));
				packet.Add(messageSequenceCount);
				udpCommunicationsManager->SendMessage(packet);
				udpCommunicationsManager->setConnectionEstablished(true);
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Connection Established"));
				if (LocalRole == EPlayerRole::AR)
				{
					arPawn->startARSession();
				}
			}
			break;
		} 
		case(EMessageType::PlayerMovement):
		{
			TSharedPtr<PlayerMovementCommand> command = MakeShared<PlayerMovementCommand>();
			command->deserialise(this, receivedPacket);
			if(udpCommunicationsManager->getConnectionEstablished() == true)
			incomingCommandQueue.Enqueue(command);
			break;
		}
		case(EMessageType::ARPlayerSelection):
		{
			TSharedPtr<arPlayerSelectionCommand> command = MakeShared<arPlayerSelectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			break;
		}
		case(EMessageType::SpawnAtSection):
		{
			TSharedPtr<SpawnAtSectionCommand> command = MakeShared<SpawnAtSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Spawn Message Received"));

			break;
		}
		case(EMessageType::InteractionAtSection):
		{
			TSharedPtr<InteractionAtSectionCommand> command = MakeShared<InteractionAtSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Interaction Message Received"));

			break;
		}
		case(EMessageType::SwitchTurns):
		{
			TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Switch Turns Message Received"));

			break;
		}
	}
}

//Sending Messages
void AARVRGameManager::AddToOutgoingCommandQueue(TSharedPtr<Command> command)
{
	if(arPlayerPlaneSelected)
	outgoingCommandQueue.Enqueue(command);
}

void AARVRGameManager::clearOutgoingQueue() {

	outgoingCommandQueue.Empty();

}

void AARVRGameManager::clearIncomingQueue() {

	incomingCommandQueue.Empty();

}

void AARVRGameManager::setupPools() {
	AObjectPoolActor* goblinPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	goblinPool->initialisePool(GoblinPooledEntityClass);
	EntityPools.Add(ESpawnableObject::Goblin, goblinPool);

	AObjectPoolActor* chestPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	chestPool->initialisePool(ChestPooledEntityClass);
	EntityPools.Add(ESpawnableObject::Chest, chestPool);
}

void AARVRGameManager::sortMapSections() 
{
	mapSections.Sort([](AMapSection& A, AMapSection& B)
		{
			FVector locA = A.GetActorLocation();
			FVector locB = B.GetActorLocation();

			if (locA.X != locB.X)
			{
				return locA.X < locB.X;
			}
			return locA.Y < locB.Y;
		});
}

void AARVRGameManager::spawnEntityAtSection(AMapSection* sectionToSpawn, ESpawnableObject objectType)
{
	switch (objectType) {
	case(ESpawnableObject::Goblin): {
			AObjectPoolActor* goblinPool = *EntityPools.Find(objectType);
			APooledEntity* goblinEntity = goblinPool->getAvailableEntity();
			if (goblinEntity && mapSections.Contains(sectionToSpawn)) {

				sectionToSpawn->spawnActorAtPoint(goblinEntity);
				sectionToSpawn->setCurrentEntity(goblinEntity);
			}
			break;
		}
		case(ESpawnableObject::Chest): {
			AObjectPoolActor* chestPool = *EntityPools.Find(objectType);
			APooledEntity* chestEntity = chestPool->getAvailableEntity();
			if (chestEntity && mapSections.Contains(sectionToSpawn)) {

				sectionToSpawn->spawnActorAtPoint(chestEntity);
				sectionToSpawn->setCurrentEntity(chestEntity);

			}
			break;
		}
	}
	if(LocalRole == EPlayerRole::AR)
	{
		TSharedPtr<SpawnAtSectionCommand> command = MakeShared<SpawnAtSectionCommand>();

		command->commandType = EMessageType::SpawnAtSection;

		command->sequenceCount = getSequenceCount();

		command->sectionIndex = mapSections.Find(sectionToSpawn);

		command->objectType = objectType;

		AddToOutgoingCommandQueue(command);

		TSharedPtr<SwitchTurnsCommand> command2 = MakeShared<SwitchTurnsCommand>();
		command2->commandType = EMessageType::SwitchTurns;

		command2->sequenceCount = getSequenceCount();

		command2->playerTurn = EPlayerRole::VR;

		AddToOutgoingCommandQueue(command2);

		switchTurns(EPlayerRole::VR);
	}
}

void AARVRGameManager::switchTurns(EPlayerRole playerTurn)
{
	switch (playerTurn) 
	{
	case(EPlayerRole::AR):
		currentTurn = playerTurn;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed: AR Player")));
		break;

	case(EPlayerRole::VR):
		currentTurn = playerTurn;
		if (currentlyOccupiedSection)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Section: %s"), *currentlyOccupiedSection->GetName()));
			currentlyOccupiedSection->toggleWalls(false);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed: VR Player 2")));
		}
		break;
	}
}

void AARVRGameManager::setLastTunnelVisited(AMapTunnel* tunnel)
{
	lastTunnelVisited = tunnel;
}

AMapTunnel* AARVRGameManager::getLastTunnelVisited()
{
	return lastTunnelVisited;
}

void AARVRGameManager::interactionConclusion(APooledEntity* concludedEntity)
{
	if (concludedEntity->IsA(AGoblinPooledEntity::StaticClass()))
	{
		AObjectPoolActor* goblinPool = *EntityPools.Find(ESpawnableObject::Goblin);
		goblinPool->returnToPool(concludedEntity);
	}
	if (concludedEntity->IsA(AChestPooledEntity::StaticClass()))
	{
		AObjectPoolActor* chestPool = *EntityPools.Find(ESpawnableObject::Chest);
		chestPool->returnToPool(concludedEntity);
	}

	TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
	command->commandType = EMessageType::SwitchTurns;

	command->sequenceCount = getSequenceCount();

	command->playerTurn = EPlayerRole::AR;

	AddToOutgoingCommandQueue(command);

	switchTurns(EPlayerRole::AR);


}