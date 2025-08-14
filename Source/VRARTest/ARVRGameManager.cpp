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
#include "VRPlayerUI.h"
#include "UIConnectionWidget.h"

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
			if (actor->Tags.Contains("MapTunnel"))
			{
				AMapTunnel* mapTunnel = Cast<AMapTunnel>(actor);
				mapTunnel->toggleArrowMesh(false);
				mapTunnels.Add(mapTunnel);
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
	currentlyOccupiedSection->setSectionUsed(true);
	currentlyOccupiedSection->setSpecialSection(true);
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

	TArray<TSharedPtr<Command>> commandsToResend;

	for (const TSharedPtr<Command>& command : waitingForConfirmationList)
	{
		if (currentTime - command->timeLastSent >= resendDelay)
		{
			commandsToResend.Add(command);
		}
	}

	for (TSharedPtr<Command>& command : commandsToResend)
	{
		command->timeLastSent = currentTime;
		ResendCommand(command);
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
				if (LocalRole == EPlayerRole::VR)
				{
					changePopUpText("Dark Wizard setting up...");
				}
				if (LocalRole == EPlayerRole::AR)
				{
					arPawn->startARSession();
					changePopUpText("Select one of the green planes, to place down the map.");
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
		case(EMessageType::BlockTunnel):
		{
			TSharedPtr<BlockTunnelCommand> command = MakeShared<BlockTunnelCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Block Tunnel Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::SwapSections):
		{
			TSharedPtr<SwapSectionCommand> command = MakeShared<SwapSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Swap Section Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::ApplyFrenzy):
		{
			TSharedPtr<ApplyFrenzyCommand> command = MakeShared<ApplyFrenzyCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Apply Frenzy Message Received"));
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
	case(EMessageType::BlockTunnel):
	case(EMessageType::SwapSections):
	case(EMessageType::ApplyFrenzy):
	{
		waitingForConfirmationList.Add(command);
		break;
	}
	}
}

void AARVRGameManager::ResendCommand(TSharedPtr<Command> command)
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
		outgoingCommandQueue.Enqueue(moveCommand);
	}
	else
	{
		outgoingCommandQueue.Enqueue(command);
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
			AObjectPoolActor** goblinPoolPtr = EntityPools.Find(objectType);
			if (goblinPoolPtr && *goblinPoolPtr)
			{
				AObjectPoolActor* goblinPool = *goblinPoolPtr;
				AActor* goblinEntity = goblinPool->getAvailableEntity();
				if (goblinEntity && mapSections.Contains(sectionToSpawn)) {
					UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(goblinEntity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
					if (pooledComponent)
					{
						if (LocalRole == EPlayerRole::VR) {
							Cast<AGoblinPooledEntity>(goblinEntity)->toggleFrenzyVR(false);
							Cast<AGoblinPooledEntity>(goblinEntity)->resetHealth();
							Cast<AGoblinPooledEntity>(goblinEntity)->CreateHealthUI();
							Cast<AGoblinPooledEntity>(goblinEntity)->EnableAI();
						}
						else if (LocalRole == EPlayerRole::AR) {
							Cast<AGoblinPooledEntity>(goblinEntity)->toggleFrenzyAR(false);
						}
						pooledComponent->setOwnerSection(sectionToSpawn);
						sectionToSpawn->spawnActorAtPoint(goblinEntity);
						sectionToSpawn->setCurrentEntity(goblinEntity);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Could not find a valid Object Pool for objectType %d"), objectType);
			}
			break;
		}
		case(ESpawnableObject::Chest): {
			AObjectPoolActor** chestPoolPtr = EntityPools.Find(objectType);
			if (chestPoolPtr && *chestPoolPtr)
			{
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
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Could not find a valid Object Pool for objectType %d"), objectType);
			}
			break;
		}
		case(ESpawnableObject::Trap): {
			AObjectPoolActor** trapPoolPtr = EntityPools.Find(objectType);
			if (trapPoolPtr && *trapPoolPtr)
			{
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
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Could not find a valid Object Pool for objectType %d"), objectType);
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
		if (LocalRole == EPlayerRole::VR)
		{
			changePopUpText("Dark Wizard's Turn");
		}
		else if (LocalRole == EPlayerRole::AR)
		{
			changePopUpText("Your Turn, place down an object or cast a spell");
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed: AR Player")));
		break;

	case(EPlayerRole::VR):
		currentTurn = playerTurn;
		if (LocalRole == EPlayerRole::VR)
		{
			changePopUpText("Your Turn");
		}
		else if (LocalRole == EPlayerRole::AR)
		{
			changePopUpText("Dwarf's Turn");
		}
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

void AARVRGameManager::interactionConclusion(TWeakObjectPtr<AActor> concludedEntityPtr)
{
	if (concludedEntityPtr.IsValid())
	{
		// It's safe to use. Get the actual actor pointer.
		AActor* concludedEntity = concludedEntityPtr.Get();

		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("InteractionConclusion for Actor: %s"), *concludedEntity->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Actor's Class is: %s"), *concludedEntity->GetClass()->GetName()));

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

		currentlyOccupiedSection->setCurrentEntity(nullptr);
		currentlyOccupiedSection->setSectionUsed(false);

		TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
		command->commandType = EMessageType::SwitchTurns;

		command->sequenceCount = getNextSequenceCount();

		command->playerTurn = EPlayerRole::AR;

		AddToOutgoingCommandQueue(command);

		switchTurns(EPlayerRole::AR);
	}
	else
	{
		// The actor was already destroyed. Log a warning and do nothing to prevent a crash.
		UE_LOG(LogTemp, Warning, TEXT("InteractionConclusion was called, but the entity was already destroyed."));
	}

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
						healthBar->CreateHealthBar(entity->getCurrentMaxHealth());
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
							livingEntity->toggleTransparent(speedPowerUpActive);
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
			if (section->getSectionUsed() == true || section->getSectionVisited() == true)
			{
				section->swapSelectedMaterial(blockedMapMaterial);
			}
			else
			{
				section->toggleArrowMesh(true);
			}
		}
		else
		{
			section->swapSelectedMaterial(regularMapMaterial);
			section->toggleArrowMesh(false);
		}
	}
}

void AARVRGameManager::displaySectionSwap(bool toggle)
{
	for (AMapSection* section : mapSections)
	{
		if (toggle == true) {
			if (section->getSectionUsed() == true)
			{
				section->toggleArrowMesh(true);
			}
			else
			{
				section->swapSelectedMaterial(blockedMapMaterial);
			}
		}
		else
		{
			section->swapSelectedMaterial(regularMapMaterial);
			section->toggleArrowMesh(false);
		}
	}
}

void AARVRGameManager::displayTunnelUsed(bool toggle)
{
	for (AMapTunnel* tunnel : mapTunnels)
	{
		if (toggle == true) {
			if (tunnel->getTunnelBlocked() == true)
			{
				tunnel->toggleArrowMesh(false);
			}
			else
			{
				tunnel->toggleArrowMesh(true);
			}
		}
		else
		{
			tunnel->toggleArrowMesh(false);
		}
	}
}

void AARVRGameManager::displaySectionFrenzy(bool toggle)
{
	for (AMapSection* section : mapSections)
	{
		if (toggle == true) {

			AActor* currentEntity = section->getCurrentEntity();

			if (currentEntity && currentEntity->IsA(ALivingPooledEntity::StaticClass()))
			{
				ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(section->getCurrentEntity());
				if (livingEntity->getIsFrenzied() == false)
				{
					section->toggleArrowMesh(true);
				}
				else
				{
					section->swapSelectedMaterial(blockedMapMaterial);
				}
			}
			else
			{
				section->swapSelectedMaterial(blockedMapMaterial);
			}
		}
		else
		{
			section->swapSelectedMaterial(regularMapMaterial);
			section->toggleArrowMesh(false);
		}
	}
}

void AARVRGameManager::sendBlockedWallCommand(AMapTunnel* tunnelToBlock)
{
	if (LocalRole == EPlayerRole::AR)
	{
		TSharedPtr<BlockTunnelCommand> command = MakeShared<BlockTunnelCommand>();

		command->commandType = EMessageType::BlockTunnel;

		command->sequenceCount = getNextSequenceCount();

		for (AMapSection* section : mapSections)
		{

			if (section->getConnectedTunnels().Contains(tunnelToBlock))
			{
				command->sectionIndex = mapSections.Find(section);

				command->tunnelIndex = section->getConnectedTunnels().Find(tunnelToBlock);

				break;
			}

		}

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

void AARVRGameManager::sendSwapSectionCommand(AMapSection* swapSelectedMapSection1, AMapSection* swapSelectedMapSection2)
{
	if (LocalRole == EPlayerRole::AR)
	{
		TSharedPtr<SwapSectionCommand> command = MakeShared<SwapSectionCommand>();

		command->commandType = EMessageType::SwapSections;

		command->sequenceCount = getNextSequenceCount();

		command->section1Index = mapSections.Find(swapSelectedMapSection1);

		command->section2Index = mapSections.Find(swapSelectedMapSection2);

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

void AARVRGameManager::sendApplyFrenzyCommand(AMapSection* selectedMapSection)
{
	if (LocalRole == EPlayerRole::AR)
	{
		TSharedPtr<ApplyFrenzyCommand> command = MakeShared<ApplyFrenzyCommand>();

		command->commandType = EMessageType::ApplyFrenzy;

		command->sequenceCount = getNextSequenceCount();

		command->sectionIndex = mapSections.Find(selectedMapSection);

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

void AARVRGameManager::changePopUpText(FString text)
{
	if (LocalRole == EPlayerRole::AR)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Changing AR text")));

		if (arPawn->getConnectionWidget()->IsVisible())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("It's Visible")));

			arPawn->getConnectionWidget()->setPopUpText(text);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("It's Not Visible")));
			arPawn->getMapSetupWidget()->setPopUpText(text);
		}
	}
	else if (LocalRole == EPlayerRole::VR)
	{
		vrCharacter->getVRPlayerUI()->setPopUpText(text);
	}
}