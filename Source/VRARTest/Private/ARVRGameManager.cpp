// Fill out your copyright notice in the Description page of Project Settings.


#include "ARVRGameManager.h"
#include "Networking/Command.h"
#include "VRSpecific/VRCharacter.h"
#include "ARSpecific/ARPawn.h"
#include "ARSpecific/ARCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Networking/UDPCommunicationsManager.h"
#include "Map/MapSection.h"
#include "ObjectPool/LivingPooledEntity.h"
#include "ObjectPool/GoblinPooledEntity.h"
#include "ObjectPool/ChestPooledEntity.h"
#include "ObjectPool/TrapPooledEntity.h"
#include "ObjectPool/SpellPooledEntity.h"
#include "Map/MapTunnel.h"
#include "UI/HealthBarWidget.h"
#include "UI/ARMapSetupUI.h"
#include "UI/VRPlayerUI.h"
#include "UI/UIConnectionWidget.h"
#include "Components/WidgetComponent.h"
#include "UI/ResultsScreenWidget.h"
#include "GameFramework/ProjectileMovementComponent.h"


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
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find UDP Communications Manager!"));
	}

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	if(LocalRole == EPlayerRole::AR) //AR specific changes
	{
		APawn* FoundPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
		arPawn = Cast<AARPawn>(FoundPawn);
		if (!arPawn)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find ARPawn"));

		}

		//All actors are looped through to find those with specific tags in order to call specific functions within them
		for (AActor* actor : AllActors) 
		{
			if (actor->Tags.Contains("MapRoot"))//Sets the mapRoot and it's original values
			{
				mapRoot = actor;
				originalMapLocation = mapRoot->GetActorLocation();
				originalMapRotation = mapRoot->GetActorRotation();
				originalMapScale = mapRoot->GetActorScale();
			}
			if (actor->Tags.Contains("ARCharacter")) //Sets the ARCharacter
			{
				arCharacter = Cast<AARCharacter>(actor);
				arCharacter->setManager(this);
				arCharacter->setIsARSession(true);
				arCharacter->SetActorTickEnabled(false);
				arCharacter->SetActorHiddenInGame(true);
			}
			if (actor->Tags.Contains("ARHide")) //Hide so they can re-enabled later
			{
				actor->SetActorHiddenInGame(true);
			}
			if (actor->Tags.Contains("MapSection")) //Adds all mapSections to an array to they can be addressd via index number and toggles walls and fog off
			{
				AMapSection* mapSection = Cast<AMapSection>(actor);
				mapSection->setManager(this);
				mapSection->toggleFog(false);
				mapSection->toggleWalls(false);
				mapSections.Add(mapSection);
			}
			if (actor->Tags.Contains("MapTunnel"))  //Adds all mapTunnels to an array to they can be addressd via index number
			{
				AMapTunnel* mapTunnel = Cast<AMapTunnel>(actor);
				mapTunnel->toggleArrowMesh(false);
				mapTunnels.Add(mapTunnel);
			}
			if (vrRepresenationClass && actor->IsA(vrRepresenationClass)) //sets the vrRepresenation
			{
				vrRepresenation = Cast<AVRRepresentative>(actor);
			}
		}
	}
	if (LocalRole == EPlayerRole::VR) //VR specific 
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AVRCharacter::StaticClass());
		vrCharacter = Cast<AVRCharacter>(FoundActor);
		if (!vrCharacter)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find VRCharacter"));

		}
		for (AActor* actor : AllActors)
		{
			if (actor->Tags.Contains("VRHide")) //hides and ets collision diables to all VRHide tagged objects
			{
				actor->SetActorHiddenInGame(true);
				actor->SetActorEnableCollision(false);
			}
			if (actor->Tags.Contains("ARCharacter")) //assigns the ARCharacter
			{
				arCharacter = Cast<AARCharacter>(actor);
				arCharacter->setManager(this);
				arCharacter->SetActorTickEnabled(false);

			}
			if (actor->Tags.Contains("MapSection")) //toggles all the walls and fog enabled on the map
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
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Connection Established"));
				if (LocalRole == EPlayerRole::VR)
				{
					changePopUpText("Dark Wizard setting up...");
				}
				if (LocalRole == EPlayerRole::AR)
				{
					if (arPawn)
					{
						arPawn->startARSession();
					}
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
		case(EMessageType::WizardMovement):
		{
			TSharedPtr<WizardMovementCommand> command = MakeShared<WizardMovementCommand>();
			command->deserialise(this, receivedPacket);
			if (udpCommunicationsManager->getConnectionEstablished() == true)
				incomingCommandQueue.Enqueue(command);
			break;
		}
		case(EMessageType::ARPlayerSelection):
		{
			TSharedPtr<arPlayerSelectionCommand> command = MakeShared<arPlayerSelectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Selection Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::SpawnAtSection):
		{
			TSharedPtr<SpawnAtSectionCommand> command = MakeShared<SpawnAtSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Spawn Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::InteractionAtSection):
		{
			TSharedPtr<InteractionAtSectionCommand> command = MakeShared<InteractionAtSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Interaction Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::SwitchTurns):
		{
			TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Switch Turns Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::UpdateHealth):
		{
			TSharedPtr<UpdateHealthCommand> command = MakeShared<UpdateHealthCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Update Health Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::BlockTunnel):
		{
			TSharedPtr<BlockTunnelCommand> command = MakeShared<BlockTunnelCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Block Tunnel Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::SwapSections):
		{
			TSharedPtr<SwapSectionCommand> command = MakeShared<SwapSectionCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Swap Section Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::ApplyFrenzy):
		{
			TSharedPtr<ApplyFrenzyCommand> command = MakeShared<ApplyFrenzyCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Apply Frenzy Message Received"));
			sendReceiptCommand(command->sequenceCount);
			break;
		}
		case(EMessageType::FireProjectile):
		{
			TSharedPtr<FireProjectileCommand> command = MakeShared<FireProjectileCommand>();
			command->deserialise(this, receivedPacket);
			incomingCommandQueue.Enqueue(command);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Fire Projectile Message Received"));
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
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("Reciept Message Received"));
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
	case(EMessageType::FireProjectile):
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
	goblinPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (goblinPool) 
	{
		goblinPool->initialisePool(GoblinPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Goblin, goblinPool);
		
	}

	chestPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (chestPool) 
	{
		chestPool->initialisePool(ChestPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Chest, chestPool);
	}

	trapPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (trapPool)
	{
		trapPool->initialisePool(TrapPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Trap, trapPool);
	}

	spellPool = GetWorld()->SpawnActor<AObjectPoolActor>();
	if (spellPool)
	{
		spellPool->initialisePool(SpellPooledEntityClass);
		EntityPools.Add(ESpawnableObject::Spell, spellPool);
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
			break;
		}
		case(ESpawnableObject::Chest): {
			
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

void AARVRGameManager::spawnNonPlacableEntity(ESpawnableObject objectType)
{
	switch (objectType)
	{
	case(ESpawnableObject::Spell):
	{
			AActor* spellEntity = spellPool->getAvailableEntity();
			if (spellEntity) 
			{
				USceneComponent* spawnPoint = arCharacter->getSpellSpawnPoint();
				FVector spawnLocation = spawnPoint->GetComponentLocation();
				FVector forwardDir = spawnPoint->GetForwardVector(); 

				spellEntity->SetActorLocation(spawnLocation);

				ASpellPooledEntity* spellPooledEntity = Cast<ASpellPooledEntity>(spellEntity);
				if (spellPooledEntity)
				{
					spellPooledEntity->setManager(this);
					spellPooledEntity->startTimeToLive();

					spellEntity->SetActorRotation(forwardDir.Rotation());

					UProjectileMovementComponent* projMove = spellPooledEntity->getProjectileMovement();
					projMove->Deactivate();
					projMove->Velocity = forwardDir * projMove->InitialSpeed;
					projMove->Activate(true);

				}
		}
		break;
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
			arPawn->getMapSetupWidget()->toggleAllButtons(true);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed: AR Player")));
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
			arPawn->getMapSetupWidget()->toggleAllButtons(false);
		}
		if (currentlyOccupiedSection)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Section: %s"), *currentlyOccupiedSection->GetName()));
			currentlyOccupiedSection->toggleWalls(false);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed: VR Player")));
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

		//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("InteractionConclusion for Actor: %s"), *concludedEntity->GetName()));
		//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Orange, FString::Printf(TEXT("Actor's Class is: %s"), *concludedEntity->GetClass()->GetName()));

		if (concludedEntity->IsA(AGoblinPooledEntity::StaticClass()))
		{
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
			chestPool->returnToPool(concludedEntity);
		}

		if (concludedEntity->IsA(ATrapPooledEntity::StaticClass()))
		{
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

void AARVRGameManager::nonPlacableObjectConclusion(TWeakObjectPtr<AActor> concludedEntityPtr)
{
	if (concludedEntityPtr.IsValid())
	{
		// It's safe to use. Get the actual actor pointer.
		AActor* concludedEntity = concludedEntityPtr.Get();

		if (concludedEntity->IsA(ASpellPooledEntity::StaticClass()))
		{
			spellPool->returnToPool(concludedEntity);
		}
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
		arCharacter->getARCombatWidget()->getDwarfHealthBar()->updateHearts(amount);
		if (amount <= 0)
		{
			arPawn->getMapSetupWidget()->SetVisibility(ESlateVisibility::Hidden);
			arCharacter->getARCombatWidget()->SetVisibility(ESlateVisibility::Hidden);
			arCharacter->getResultsScreenWidget()->SetVisibility(ESlateVisibility::Visible);
			arCharacter->getResultsScreenWidget()->setResultText("YOU WIN");
		}
	}
}

void AARVRGameManager::updateEnemyHealth(float amount)
{
	if (LocalRole == EPlayerRole::AR) {
		if (wizardCombatActive == false)
		{
			arPawn->getMapSetupWidget()->getOtherHealthBar()->updateHearts(amount);
			if (amount <= 0)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Enemy Destroyed")));
				arPawn->getMapSetupWidget()->getOtherHealthBar()->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			arCharacter->getARCombatWidget()->getDamageFlashWidget()->PlayFlash();
			arCharacter->getARCombatWidget()->getWizardHealthBar()->updateHearts(amount);
			if (amount <= 0)
			{
				arCharacter->getARCombatWidget()->SetVisibility(ESlateVisibility::Hidden);
				arCharacter->getPlayerController()->ActivateTouchInterface(nullptr);
				arCharacter->getResultsScreenWidget()->SetVisibility(ESlateVisibility::Visible);
				arCharacter->getResultsScreenWidget()->setResultText("YOU LOSE");
			}
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
			if (section->getSectionUsed() == true || section == currentlyOccupiedSection)
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
			if (tunnel->getTunnelBlocked() == true || tunnel->getSpeciaTunnel() == true)
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
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Changing AR text")));

		if (arPawn->getConnectionWidget()->IsVisible())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("It's Visible")));

			arPawn->getConnectionWidget()->setPopUpText(text);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("It's Not Visible")));
			arPawn->getMapSetupWidget()->setPopUpText(text);
		}
	}
	else if (LocalRole == EPlayerRole::VR)
	{
		vrCharacter->getVRPlayerUI()->setPopUpText(text);
	}
}

void AARVRGameManager::startWizardCombat()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	arPawn->stopARSession();

	arPawn->SetActorTickEnabled(false);
	arPawn->getMapSetupWidget()->SetVisibility(ESlateVisibility::Hidden);

	mapRoot->SetActorScale3D(originalMapScale);
	mapRoot->SetActorLocation(originalMapLocation);
	mapRoot->SetActorRotation(originalMapRotation);

	arCharacter->SetActorTickEnabled(true);
	arCharacter->SetActorHiddenInGame(false);
	arCharacter->getARCombatWidget()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	playerController->Possess(arCharacter);
	playerController->SetViewTarget(arCharacter);


	wizardCombatActive = true;
}
