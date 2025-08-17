// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPoolActor.h"
#include "VRRepresentative.h"
class AVRCharacter;
class AARPawn;
class AARCharacter;
class Command;
class PlayerMovementCommand;
class AUDPCommunicationsManager;
class AMapSection;
class AMapTunnel;
#include "ARVRGameManager.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	VR UMETA(DisplayName = "VR"),
	AR UMETA(DisplayName = "AR")
};

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

UENUM(BlueprintType)
enum class ESpawnableObject : uint8
{
	Goblin UMETA(DisplayName = "Goblin"),
	Chest UMETA(DisplayName = "Chest"),
	Trap UMETA(DisplayName = "Trap"),
	Spell UMETA(DisplayName = "Spell"),
	None UMETA(DisplayName = "None")
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

	virtual void Tick(float DeltaTime) override;

	void ManageCommandQueue();

	void AddToIncomingCommandQueue(TArray<uint8> packet);

	void AddToOutgoingCommandQueue(TSharedPtr<Command> command);

	void ResendCommand(TSharedPtr<Command> command);

	uint32 getSequenceCount() const { return messageSequenceCount; };

	uint32 getNextSequenceCount() { return messageSequenceCount++; };

	AActor* GetMapRoot() { return mapRoot; };

	void clearOutgoingQueue();

	void clearIncomingQueue();

	void setupPools();

	void sortMapSections();

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

	void interactionConclusion(TWeakObjectPtr<AActor> concludedEntityPtr);

	void spawnNonPlacableEntity(ESpawnableObject objectType);

	void nonPlacableObjectConclusion(TWeakObjectPtr<AActor> concludedEntityPtr);

	void switchTurns(EPlayerRole playerTurn);

	void updatePlayerHealth(float amount);

	void updateEnemyHealth(float amount);

	void handleNextSection(AMapTunnel* tunnel, bool speedPowerUp);

	void displaySectionUsed(bool toggle);

	void displaySectionSwap(bool toggle);

	void displayTunnelUsed(bool toggle);

	void displaySectionFrenzy(bool toggle);

	void sendBlockedWallCommand(AMapTunnel* tunnelToBlock);

	void sendSwapSectionCommand(AMapSection* swapSelectedMapSection1, AMapSection* swapSelectedMapSection2);

	void sendApplyFrenzyCommand(AMapSection* selectedMapSection);

	void changePopUpText(FString text);

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

	void sendReceiptCommand(uint32 sequence);

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* blockedMapMaterial;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* regularMapMaterial;

	bool wizardCombatActive = false;
};
