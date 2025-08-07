// Fill out your copyright notice in the Description page of Project Settings.


#include "ARVRGameManager.h"
#include "Command.h"
#include "VRCharacter.h"
#include "ARPawn.h"
#include "Kismet/GameplayStatics.h"
#include "UDPCommunicationsManager.h"
#include "MapSection.h"
#include "LivingPooledEntity.h"
#include "GoblinPooledEntity.h"
#include "ChestPooledEntity.h"
#include "TrapPooledEntity.h"
#include "MapTunnel.h"
#include "HealthBarWidget.h"
#include "ARMapSetupUI.h"
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
				mapSection->toggleWalls(false);
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
				mapSection->toggleWalls(true);
				mapSections.Add(mapSection);
			}
		}

	}
	sortMapSections();
	currentlyOccupiedSection = mapSections[0];
	currentlyOccupiedSection->toggleFog(false);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Starting Section: %s"), *currentlyOccupiedSection->GetName()));

	setupPools();
}

// Called every frame
void AARVRGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ManageCommandQueue();
	float currentTime = GetWorld()->GetTimeSeconds();
	const float resendDelay = 1.0f;
	for (TSharedPtr<Command>& command : waitingForConfirmationList)
	{
		if (currentTime - command->timeLastSent >= resendDelay)
		{
			AddToOutgoingCommandQueue(command);
			command->timeLastSent = currentTime;
		}
	}

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
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Selection Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::SpawnAtSection):
		{
			TSharedPtr<SpawnAtSectionCommand> command = MakeShared<SpawnAtSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Spawn Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::InteractionAtSection):
		{
			TSharedPtr<InteractionAtSectionCommand> command = MakeShared<InteractionAtSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Interaction Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::SwitchTurns):
		{
			TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Switch Turns Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::UpdateHealth):
		{
			TSharedPtr<UpdateHealthCommand> command = MakeShared<UpdateHealthCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Update Health Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::ReceiptConfirmation):
		{
			TSharedPtr<ReceiptConfirmationCommand> command = MakeShared<ReceiptConfirmationCommand>();
			command->deserialise(this, receivedPacket);
			for (int32 i = 0; i < waitingForConfirmationList.Num(); ++i)
			{
				if (waitingForConfirmationList[i]->sequenceCount == command->sequenceOfReceipt)
				{
					waitingForConfirmationList.RemoveAt(i);
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Reciept Message Received"));
					break;
				}
			}
			break;
		}
	}
}

//Sending Messages
void AARVRGameManager::AddToOutgoingCommandQueue(TSharedPtr<Command> command)
{
	if (!arPlayerPlaneSelected) 
	{
		return;
	}
	if (command->commandType == EMessageType::PlayerMovement)
	{
		TSharedPtr<PlayerMovementCommand> moveCommand = StaticCastSharedPtr<PlayerMovementCommand>(command);
		if (currentlyOccupiedSection->getCurrentEntity()) {
			AActor* currentEntity = currentlyOccupiedSection->getCurrentEntity();
			if (currentEntity->IsA(ALivingPooledEntity::StaticClass()))
			{
				ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(currentEntity);
				moveCommand->entityIncluded = true;
				moveCommand->entityPosition = livingEntity->getEntityPosition();
				moveCommand->entityRotation = livingEntity->getEntityRotation();

			}
			else
			{
				moveCommand->entityIncluded = false;
			}
		}
		else 
		{
			moveCommand->entityIncluded = false;
		}
		moveCommand->timeLastSent = GetWorld()->GetTimeSeconds();
		outgoingCommandQueue.Enqueue(moveCommand);
	
	}
	else
	{ 
		command->timeLastSent = GetWorld()->GetTimeSeconds();
		outgoingCommandQueue.Enqueue(command);
	}

	switch (command->commandType)
	{
	case(EMessageType::ARPlayerSelection):
	case(EMessageType::SpawnAtSection):
	case(EMessageType::InteractionAtSection):
	case(EMessageType::SwitchTurns):
	case(EMessageType::UpdateHealth):
	{
		waitingForConfirmationList.Add(command);
		break;
	}
	}
}
	
void AARVRGameManager::clearOutgoingQueue() {

	outgoingCommandQueue.Empty();

}

void AARVRGameManager::clearIncomingQueue() {

	incomingCommandQueue.Empty();

}

void AARVRGameManager::setupPools() {
	AObjectPoolActor* goblinPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (goblinPool) 
	{
		goblinPool->initialisePool(GoblinPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Goblin, goblinPool);
		
	}

	AObjectPoolActor* chestPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (chestPool) 
	{
		chestPool->initialisePool(ChestPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Chest, chestPool);
	}

	AObjectPoolActor* trapPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (trapPool)
	{
		trapPool->initialisePool(TrapPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Trap, trapPool);
	}
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
			AActor* goblinEntity = goblinPool->getAvailableEntity();
			if (goblinEntity && mapSections.Contains(sectionToSpawn)) {
				UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(goblinEntity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
				if (pooledComponent)
				{
					pooledComponent->setOwnerSection(sectionToSpawn);
					sectionToSpawn->spawnActorAtPoint(goblinEntity);
					sectionToSpawn->setCurrentEntity(goblinEntity);
					if (LocalRole == EPlayerRole::VR) {
						Cast<AGoblinPooledEntity>(goblinEntity)->resetHealth();
						Cast<AGoblinPooledEntity>(goblinEntity)->CreateHealthUI();
					}
				}
			}
			break;
		}
		case(ESpawnableObject::Chest): {
			AObjectPoolActor* chestPool = *EntityPools.Find(objectType);
			AActor* chestEntity = chestPool->getAvailableEntity();
			if (chestEntity && mapSections.Contains(sectionToSpawn)) {
				UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(chestEntity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
				if (pooledComponent)
				{
					Cast<AChestPooledEntity>(chestEntity)->resetChest();
					pooledComponent->setOwnerSection(sectionToSpawn);
					sectionToSpawn->spawnActorAtPoint(chestEntity);
					sectionToSpawn->setCurrentEntity(chestEntity);
				}
			}
			break;
		}
		case(ESpawnableObject::Trap): {
			AObjectPoolActor* trapPool = *EntityPools.Find(objectType);
			AActor* trapEntity = trapPool->getAvailableEntity();
			if (trapEntity && mapSections.Contains(sectionToSpawn)) {
				UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(trapEntity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
				if (pooledComponent)
				{
					Cast<ATrapPooledEntity>(trapEntity)->resetTrap();
					pooledComponent->setOwnerSection(sectionToSpawn);
					sectionToSpawn->spawnActorAtPoint(trapEntity);
					sectionToSpawn->setCurrentEntity(trapEntity);
					if (LocalRole == EPlayerRole::VR)
					{
						Cast<ATrapPooledEntity>(trapEntity)->toggleMeshVisibility(false);
					}
				}
			}
			break;
		}
	}
	sectionToSpawn->setSectionUsed(true);
	if(LocalRole == EPlayerRole::AR)
	{
			TSharedPtr<SpawnAtSectionCommand> command = MakeShared<SpawnAtSectionCommand>();

			command->commandType = EMessageType::SpawnAtSection;

			command->sequenceCount = getNextSequenceCount();

			command->sectionIndex = mapSections.Find(sectionToSpawn);

			command->objectType = objectType;

			AddToOutgoingCommandQueue(command);

		if (arPawn->getMapSetup() == true) {
			TSharedPtr<SwitchTurnsCommand> command2 = MakeShared<SwitchTurnsCommand>();
			command2->commandType = EMessageType::SwitchTurns;

			command2->sequenceCount = getNextSequenceCount();

			command2->playerTurn = EPlayerRole::VR;

			AddToOutgoingCommandQueue(command2);

			switchTurns(EPlayerRole::VR);
		}
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
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed: VR Player")));
		}
		if (LocalRole == EPlayerRole::VR) {
			if (vrCharacter->getSpeedPowerDown() == true)
			{
				bool check = vrCharacter->speedPowerDownCheck();
				if (check == true)
				{
					TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
					command->commandType = EMessageType::SwitchTurns;

					command->sequenceCount = getNextSequenceCount();

					command->playerTurn = EPlayerRole::AR;

					AddToOutgoingCommandQueue(command);

					switchTurns(EPlayerRole::AR);
				}
			}
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

void AARVRGameManager::interactionConclusion(AActor* concludedEntity)
{
	if (concludedEntity->IsA(AGoblinPooledEntity::StaticClass()))
	{
		AObjectPoolActor* goblinPool = *EntityPools.Find(ESpawnableObject::Goblin);
		goblinPool->returnToPool(concludedEntity);
		if (LocalRole == EPlayerRole::VR)
		{
			if (vrCharacter->getHammerPowerUp() == true)
			{
				vrCharacter->turnOffHammerPowerUp();
			}
			if (vrCharacter->getHammerPowerDown() == true)
			{
				vrCharacter->turnOffHammerPowerDown();
			}
		}
	}
	if (concludedEntity->IsA(AChestPooledEntity::StaticClass()))
	{
		AObjectPoolActor* chestPool = *EntityPools.Find(ESpawnableObject::Chest);
		chestPool->returnToPool(concludedEntity);
	}

	if (concludedEntity->IsA(ATrapPooledEntity::StaticClass()))
	{
		AObjectPoolActor* trapPool = *EntityPools.Find(ESpawnableObject::Trap);
		trapPool->returnToPool(concludedEntity);
	}

	TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
	command->commandType = EMessageType::SwitchTurns;

	command->sequenceCount = getNextSequenceCount();

	command->playerTurn = EPlayerRole::AR;

	AddToOutgoingCommandQueue(command);

	switchTurns(EPlayerRole::AR);


}

void AARVRGameManager::setCurrentlyOccupiedSection(AMapSection* section)
{
	currentlyOccupiedSection = section;
	if (LocalRole == EPlayerRole::AR) {
		if (section->getCurrentEntity()) {
			if (section->getCurrentEntity()->ActorHasTag("Enemy"))
			{
				ALivingPooledEntity* entity = Cast<ALivingPooledEntity>(section->getCurrentEntity());

				UHealthBarWidget* healthBar = arPawn->getMapSetupWidget()->getOtherHealthBar();

				if (entity->IsA(AGoblinPooledEntity::StaticClass()))
				{
					if (healthBar->Visibility == ESlateVisibility::Hidden) {
						healthBar->CreateHealthBar(entity->getMaxHealth());
						healthBar->setHealthBarHeaderText("Goblin Health:");
						healthBar->SetVisibility(ESlateVisibility::Visible);
					}
				}

			}
		}
	}
}

void AARVRGameManager::updatePlayerHealth(float amount)
{
	if (LocalRole == EPlayerRole::AR) {
		arPawn->getMapSetupWidget()->getDwarfHealthBar()->updateHearts(amount);
	}
}

void AARVRGameManager::updateEnemyHealth(float amount)
{
	if (LocalRole == EPlayerRole::AR) {
		arPawn->getMapSetupWidget()->getOtherHealthBar()->updateHearts(amount);
		if (amount <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Enemy Destroyed")));
			arPawn->getMapSetupWidget()->getOtherHealthBar()->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AARVRGameManager::handleNextSection(AMapTunnel* tunnel, bool speedPowerUp)
{
	bool speedPowerUpActive = false;
	if (speedPowerUp)
	{
		speedPowerUpActive = vrCharacter->speedPowerUpCheck();
	}
	for (AMapSection* section : mapSections)
	{

		if (section->getConnectedTunnels().Contains(tunnel))
		{

			if (section != currentlyOccupiedSection) {

				section->toggleFog(true);
				tunnel->turnOnFog(false);

				AActor* currentEntity = section->getCurrentEntity();

				if (currentEntity) {

					FVector entityLocation = currentEntity->GetActorLocation();
					FVector targetLocation = tunnel->GetActorLocation();

					FRotator lookAtRotation = (targetLocation - entityLocation).Rotation();

					FRotator flatRotation = FRotator(0.f, lookAtRotation.Yaw, 0.f);

					currentEntity->SetActorRotation(flatRotation);

						if (ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(currentEntity))
						{
							//livingEntity->toggletr  // Example method
						}
						else if (AStaticPooledEntity* staticEntity = Cast<AStaticPooledEntity>(currentEntity))
						{
								staticEntity->toggleTransparent(speedPowerUpActive);
						}

				}
			}
		}
	}
}

void AARVRGameManager::sendReceiptCommand(uint32 sequence)
{
	TSharedPtr<ReceiptConfirmationCommand> command = MakeShared<ReceiptConfirmationCommand>();
	command->commandType = EMessageType::ReceiptConfirmation;

	command->sequenceCount = getNextSequenceCount();

	command->sequenceOfReceipt = sequence;

	AddToOutgoingCommandQueue(command);
}

void AARVRGameManager::displaySectionUsed(bool toggle)
{
	for (AMapSection* section : mapSections)
	{
		if (toggle == true) {
			if (section->getSectionUsed() == true || section->getSectionVisited())
			{
				section->swapSelectedMaterial(blockedMapMaterial);
			}
		}
		else
		{
			section->swapSelectedMaterial(regularMapMaterial);
		}
	}
}
