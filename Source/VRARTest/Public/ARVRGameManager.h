// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPool/ObjectPoolActor.h"
#include "ARSpecific/VRRepresentative.h"
class AVRCharacter;
class AARPawn;
class AARCharacter;
class Command;
class PlayerMovementCommand;
class AUDPCommunicationsManager;
class AMapSection;
class AMapTunnel;
#include "ARVRGameManager.generated.h"

// This enum class denotes to the two different player roles in the system, this is used throughout this class to apply a seperation in functionality for some methods
// allowing code to work for both users.
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	VR,
	AR
};

//This enum depicts the different message types that can be sent over the network.
UENUM(BlueprintType)
enum class EMessageType : uint8
{
	Connection,
	PlayerMovement,
	WizardMovement,
	ObjectStateChange,
	ARPlayerSelection,
	SpawnAtSection,
	SwitchTurns,
	InteractionAtSection,
	UpdateHealth,
	BlockTunnel,
	SwapSections,
	ApplyFrenzy,
	FireProjectile,
	ReceiptConfirmation

};

//This enum class shows all the different spawnable options for the AR Player
UENUM(BlueprintType)
enum class ESpawnableObject : uint8
{
	Goblin,
	Chest,
	Trap,
	Spell,
	None
};

UCLASS()
class VRARTEST_API AARVRGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	AARVRGameManager();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AR/VR")
	EPlayerRole LocalRole;

	EPlayerRole currentTurn;

	EPlayerRole getCurrentTurn()
	{
		return currentTurn;
	}


	AUDPCommunicationsManager* udpCommunicationsManager;

	AARPawn* arPawn;

	AARCharacter* arCharacter;

	AARCharacter* getARCharacter()
	{
		return arCharacter;
	}

	AVRCharacter* vrCharacter;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AVRRepresentative> vrRepresenationClass;
	
	AVRRepresentative* vrRepresenation;

	AVRRepresentative* getVRRep() { return vrRepresenation; };

	//Within the Tick function of the ARVRManager lies functionality for calling the manageCommandQueue() every frame in order to deal with incoming commands sent over the network,
	// and outgoing ones that need to be sent, as well as checking for receipt messages from the other player, checking for the need to resend messages incase a packet got lost.
	virtual void Tick(float DeltaTime) override;

	// ManageCommandQueue() is a function responsible for redirecting commands at the end of their lifecycle in the system, having access to both the incomingCommandQueue and the
	// outgoingCommandQueue, every frame it checks whether there are commands within both, if there are commands in the outgoing it calls the SendMessage() command in the 
	// UDPCommunicationsManager in order to send the serialised command to the other player. If the incomingCommandQueue isn't empty it will execute whatever commands are within.
	void ManageCommandQueue();

	// AddToIncomingCommandQueue() is the function that deals with packets recieved by the UDPCommunicationsManager, it will first check the first byte containing the messageType
	// and then will use a switch case to determine which command type the packet needs to be deserialised into, it will then add this newly created command to the incomingQueue and 
	// if necessary create and send a reciept command back to the other player.
	void AddToIncomingCommandQueue(TArray<uint8> packet);

	//AddToOutgoingCommandQueue receives a shared pointer to a command, it will then check the command type to determine if more needs to be added to the command before it can be sent out,
	// if it is a PlayerMovementCommand a check will be made to see if the current section their in also contains a living entity, if it does their positon will be pooled into the command.
	// If the command needs confirmation timeLastSent will be added and these commands will be added to the waitingForConfirmationList.
	void AddToOutgoingCommandQueue(TSharedPtr<Command> command);

	//If on the offchance the system does not receive confirmation of receipt from one of their commands, the command will be re-sent the logic of this function follows the logic
	// of AddToOutgoingCommandQueue() closely
	void ResendCommand(TSharedPtr<Command> command);

	uint32 getSequenceCount() const { return messageSequenceCount; };

	uint32 getNextSequenceCount() { return messageSequenceCount++; };

	AActor* GetMapRoot() { return mapRoot; };

	void clearOutgoingQueue();

	void clearIncomingQueue();

	// The setupPools() function will go through each of the different spawnableObject types and initialise a pool for it and add that pool to a map of pools and SpawnableObjects, EntityPools,
	// this way pools can be retreived from the ESpawnableObject type itself.
	void setupPools();

	// This code sorts all of the map sections by their position in world space, ordering them first by their y coordinate and then by there x, the first index in the array having the lowest x and y
	// value and the last index having the highest y. This syncs up the two arrays between systems for easy reference of sections.
	void sortMapSections();

	// spawnEntityAtSection() function just takes the object type and if possible retrieves an entity from the pool associated, it will then turn this entity on and apply any other necessary changes
	// to ensure it's functionality, it will then spawn this retreived object in the spawn point of the section also passed into the funtion. After this is done a SpawnAtSectionCommand is sent to 
	// replicate this operation on the VR system and a SwitchTurnsCommand is also sent as spawning objects indicates the end of the AR Players turn.
	void spawnEntityAtSection(AMapSection* sectionToSpawn, ESpawnableObject objectType);

	bool arPlayerPlaneSelected = false;

	TArray<AMapSection*> getMapSections() 
	{
		return mapSections;
	}

	AMapSection* currentlyOccupiedSection;

	void setCurrentlyOccupiedSection(AMapSection* section);
	
	AMapSection* getCurrentlyOccupiedSection()
	{
		return currentlyOccupiedSection;
	}


	AMapTunnel* lastTunnelVisited;

	void setLastTunnelVisited(AMapTunnel* tunnel);

	AMapTunnel* getLastTunnelVisited();

	bool vrPlayerTurn = false;

	// interactionConclusion receives a weak pointer to an actor that needs to be returned to it's ObjectPool. This also handles the experiation of any 
	// powerups the VR Player may have that end after an interaction. A SwitchTurnsCommand is sent aswell as this implies the end of the VR players turn.
	void interactionConclusion(TWeakObjectPtr<AActor> concludedEntityPtr);

	// spawnNonPlacableEntity()'s logic is essentially the same as spawnEntityAtSection() except it will spawn them at a specific point instead of at a mapSection
	// this is also used to start the time to live of objects if that functionality is required. 
	void spawnNonPlacableEntity(ESpawnableObject objectType);

	// nonPlacableObjectConclusion()'s logic is essentially the same as interactionConclusion() simply returning the object to it's objectPool.
	void nonPlacableObjectConclusion(TWeakObjectPtr<AActor> concludedEntityPtr);

	//switchTurns is a direct call to handle logic that happens at turn switch such as changing UI elemsnts to reflect the changing
	// and toggling the tunnel walls, this also checks whether the VRPlayer has a speedup power up active and will delay sending a SwitchTurnsCommand
	// if they do.
	void switchTurns(EPlayerRole playerTurn);

	// updatePlayerHealth is used by the AR player after receiving a UpdateHealthCommand this will make ammendments to all UI elements and check to see if the win condition
	// has been achieved by setting the VR Players health to zero.
	void updatePlayerHealth(float amount);

	//updateEnemyHealth will check if the AR Player is currently in Wizard Combat indicating the enemy refers to the AR Player themselves, it will update the health regardless
	// of livingentity or ARCharacter being effected. It will also check to see if the lose condition has been achieved by reducing the AR Players health to 0.
	void updateEnemyHealth(float amount);

	//handleNextSection() will iterate through all of the map sections to find those that contain the tunnel given in the parameter in the mapTunnels array, if it does and it is
	// not the current map section it would imply it is the room the VR player is about to approach this will then change the direction of the entity within to face the VRCharacter.
	// This will also handle setting the entity to transparent if the speedPowerUp is active for the VR Player.
	void handleNextSection(AMapTunnel* tunnel, bool speedPowerUp);

	// displaySectionUsed() will use the value of the boolean to determine whether it will display a block mapsection material on all the sections where getSectionUsed()
	// returns true, also changing the visibility of the arrow above it to imply to the AR Player this section is selectable or not.
	void displaySectionUsed(bool toggle);

	// displaySectionSwap() will use the value of the boolean to determine whether it will display a block mapsection material on all the sections that do not contain a currentEntity,
	// also changing the visibility of the arrow above it to imply to the AR Player this section is selectable or not.
	void displaySectionSwap(bool toggle);

	// displayTunnelUsed() will use the value of the boolean to determine whether it will display a block mapTunnel material on all the sections where getTunnelBlocked()
	// returns true, also changing the visibility of the arrow above it to imply to the AR Player this section is selectable or not.
	void displayTunnelUsed(bool toggle);

	// displaySectionFrenzy() will use the value of the boolean to determine whether it will display a block mapsection material on all the sections that do not contain a LivingEntity,
	// also changing the visibility of the arrow above it to imply to the AR Player this section is selectable or not.
	void displaySectionFrenzy(bool toggle);

	// sendBlockedWallCommand() is a helper function used to create and send BlockTunnelCommands
	void sendBlockedWallCommand(AMapTunnel* tunnelToBlock);

	// sendSwapSectionCommand() is a helper function used to create and send SwapSectionCommands
	void sendSwapSectionCommand(AMapSection* swapSelectedMapSection1, AMapSection* swapSelectedMapSection2);

	// sendApplyFrenzyCommand() is a helper function used to create and send ApplyFrenzyCommands
	void sendApplyFrenzyCommand(AMapSection* selectedMapSection);

	void changePopUpText(FString text);

	//startWizardCombat() is called when the VR Player enters the final room this will turn off the ARSession and swtich position of the AR PlayerController from the arPawn to 
	// the arCharacter, also resetting the map size and disabling old UI Widget and enabling the ARCombatUI
	void startWizardCombat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
private:	
	
	TQueue<TSharedPtr<Command>> incomingCommandQueue;

	TQueue<TSharedPtr<Command>> outgoingCommandQueue;


	uint32 messageSequenceCount = 0;

	AActor* mapRoot;

	FVector originalMapLocation;

	FRotator originalMapRotation;

	FVector originalMapScale;
	
	UPROPERTY()
	TMap<ESpawnableObject, AObjectPoolActor*> EntityPools;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AActor> GoblinPooledEntityClass;

	UPROPERTY()
	AObjectPoolActor* goblinPool;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AActor> ChestPooledEntityClass;

	UPROPERTY()
	AObjectPoolActor* chestPool;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AActor> TrapPooledEntityClass;

	UPROPERTY()
	AObjectPoolActor* trapPool;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AActor> SpellPooledEntityClass;

	UPROPERTY()
	AObjectPoolActor* spellPool;

	TArray<AMapSection*> mapSections;

	TArray <AMapTunnel*> mapTunnels;

	TArray<TSharedPtr<Command>> waitingForConfirmationList;

	// sendApplyFrenzyCommand() is a helper function used to create and send ReceiptCommands
	void sendReceiptCommand(uint32 sequence);

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* blockedMapMaterial;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* regularMapMaterial;

	bool wizardCombatActive = false;
};
