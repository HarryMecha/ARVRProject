// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPoolActor.h"
class AVRCharacter;
class AARPawn;
class Command;
class PlayerMovementCommand;
class AUDPCommunicationsManager;
class AMapSection;
class AMapTunnel;
#include "ARVRGameManager.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	VR,
	AR
};


UCLASS()
class VRARTEST_API AARVRGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties

	enum class EMessageType : uint8
	{
		Connection = 1,
		PlayerMovement = 2,
		ObjectStateChange = 3,
		ARPlayerSelection = 4,
		SpawnAtSection = 5,
		InteractionAtSection = 6
	};

	enum class ESpawnableObject : uint8
	{
		Goblin = 1,
		Skeleton = 2,
		Demon = 3,
		Chest = 4,
		Trap = 5
	};

	AARVRGameManager();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AR/VR")
	EPlayerRole LocalRole;

	AUDPCommunicationsManager* udpCommunicationsManager;

	AARPawn* arPawn;

	AVRCharacter* vrCharacter;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AActor> vrRepresenationClass;
	
	AActor* vrRepresenation;

	AActor* getVRRep() { return vrRepresenation; };

	virtual void Tick(float DeltaTime) override;

	void ManageCommandQueue();

	void AddToIncomingCommandQueue(TArray<uint8> packet);

	void AddToOutgoingCommandQueue(TSharedPtr<Command> command);

	uint32 getSequenceCount() const { return messageSequenceCount; };

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

	void setCurrentlyOccupiedSection(AMapSection* section)
	{
		currentlyOccupiedSection = section;
	}

	AMapSection* getCurrentlyOccupiedSection()
	{
		return currentlyOccupiedSection;
	}


	AMapTunnel* lastTunnelVisited;

	void setLastTunnelVisited(AMapTunnel* tunnel);


	AMapTunnel* getLastTunnelVisited();


	bool vrPlayerTurn = false;

	void interactionConclusion(APooledEntity* concludedEntity);

	void switchTurns();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
private:	
	
	TQueue<TSharedPtr<Command>> incomingCommandQueue;

	TQueue<TSharedPtr<Command>> outgoingCommandQueue;


	uint32 messageSequenceCount;

	AActor* mapRoot;

	TMap<ESpawnableObject, AObjectPoolActor*> EntityPools;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<APooledEntity> GoblinPooledEntityClass;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<APooledEntity> ChestPooledEntityClass;

	TArray<AMapSection*> mapSections;
};
