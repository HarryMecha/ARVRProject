// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapTunnel.h"
class ATorchActor;
#include "PooledEntityInterface.h"
#include "MapSection.generated.h"

UCLASS()
class VRARTEST_API AMapSection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapSection();

	virtual void Tick(float DeltaTime) override;

	void swapSelectedMaterial(UMaterialInterface* materialToSwap);

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
	TArray<AMapTunnel*> connectedTunnels; 

	TArray<AMapTunnel*> getConnectedTunnels()
	{
		return connectedTunnels;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch Array")
	TArray<ATorchActor*> torchArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* boxColliderSection;

	void toggleFog(bool toggle);

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

	AActor* currentEntity;

	void setCurrentEntity(AActor* entity)
	{
		currentEntity = entity;
		IPooledEntityInterface* poolInterface = Cast<IPooledEntityInterface>(currentEntity);
		if (poolInterface)
		{
			poolInterface->setOwnerSection(this);
		}
	}

	AActor* getCurrentEntity()
	{
		return currentEntity;
	}

	void interactionConclusion();

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
