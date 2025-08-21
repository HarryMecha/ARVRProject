// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map/MapTunnel.h"
class ATorchActor;
#include "ObjectPool/PooledEntityInterface.h"
#include "MapSection.generated.h"

UCLASS()
class VRARTEST_API AMapSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapSection();

	virtual void Tick(float DeltaTime) override;

	// swapSelectedMaterial() swaps takes a mateiralInterface and swaps the currrent current material of the map section with this one.
	void swapSelectedMaterial(UMaterialInterface* materialToSwap);

	// spawnActorAtPoint() will take in an actor in it's parameter it will then set the actors location to the spawnPoint SceneComponent in the center of the section.
	void spawnActorAtPoint(AActor* actorToSpawn);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* mapMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* arrowMesh;

	void toggleArrowMesh(bool toggle)
	{
		if (arrowMesh)
		{
			arrowMesh->SetVisibility(toggle);
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	class USceneComponent* spawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tunnels")
	TArray<AMapTunnel*> connectedTunnels; // These are all the tunnel actors that link this section to the next.

	TArray<AMapTunnel*> getConnectedTunnels()
	{
		return connectedTunnels;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch Array")
	TArray<ATorchActor*> torchArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* boxColliderSection;

	// toggleFog() iterates through the array of MapTunnels in connectedTunnels and calls their turnOnFog() function, using the boolean passed in.
	void toggleFog(bool toggle);
	
	// toggleFog() iterates through the array of MapTunnels in connectedTunnels and calls either raiseAllWalls() or resetAllWalls() function, depending on the valeu
	// of the boolean passed in.
	void toggleWalls(bool toggle);

	class AARVRGameManager* arvrmanager;

	void setManager(AARVRGameManager* manager)
	{
		arvrmanager = manager;
		for (AMapTunnel* tunnel : connectedTunnels)
		{
			tunnel->setManager(manager);
		}
	}

	UPROPERTY()
	AActor* currentEntity;

	UFUNCTION()
	void setCurrentEntity(AActor* entity)
	{
		currentEntity = entity;
		IPooledEntityInterface* poolInterface = Cast<IPooledEntityInterface>(currentEntity);
		if (poolInterface)
		{
			poolInterface->setOwnerSection(this);
		}
	}

	UFUNCTION()
	AActor* getCurrentEntity()
	{
		return currentEntity;
	}

	// interactionConclusion() is called when the VR user concludes their intreaction with the entity contained in this room, it will call the interactionConclusion() function in the
	// ARVRManager that returns the entity to it's respective pool. Meaning the currentEntity within this section is set to nullptr and it's marked as no in use, this also sends a 
	// InteractionAtSection command over the network to sync up states between games.
	void interactionConclusion();

	// toggleArrows() controls the visibility of the arrows actors in the section.
	void toggleArrows(bool toggle);

	bool getSectionUsed()
	{
		return sectionUsed;
	}

	void setSectionUsed(bool toggle)
	{
		sectionUsed = toggle;
	}

	bool getSectionVisited()
	{
		return sectionVisited;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Section")
	bool specialSection = false;

	bool getSpecialSection()
	{
		return specialSection;
	}

	void setSpecialSection(bool toggle)
	{
		specialSection = toggle;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Section")
	bool finalSection = false;

	bool getFinalSection()
	{
		return specialSection;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* selectedMapMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* regularMapMaterial;

	// OverlapBegin() is the default function for when the VR Player enters the boxCollider attached to the mapSection, it will inform the ARVRManager that this is now the currentMapSection and 
	// will call the toggleWalls() funtion. This function also checks whether the overlapped actor is the VR Character and if they are they will change the visibility of the marker on their map
	// widget indicating they've visited this section. If the map section a command will be sent to switch turns.
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Torch")
	USceneComponent* torchHolder;


private:	

	bool isSelected;

	bool sectionUsed;

	bool fogOn = false;

	bool sectionVisited = false;

};
