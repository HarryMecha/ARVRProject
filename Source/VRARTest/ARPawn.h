// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARBlueprintLibrary.h"
#include "ARSessionConfig.h"
#include "MapSection.h"
#include "ARVRGameManager.h"
#include "GameFramework/Pawn.h"

class UUIObserver;
class UUIConnectionWidget;
class UARMapSetupUI;

#include "ARPawn.generated.h"


UCLASS()
class VRARTEST_API AARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AARPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UUserWidget* getConnectionWidget() const;

	UARMapSetupUI* getMapSetupWidget() const;

	void resetARState();

	void ConfirmMapChoice();

	void startARSession();

	void setObjectToSpawn(ESpawnableObject objectType)
	{
		objectToSpawn = objectType;
		if (objectToSpawn == ESpawnableObject::None)
		{
			manager->displaySectionUsed(false);
			manager->displayTunnelUsed(false);
			manager->displaySectionSwap(false);
			currentlySelectedMapSection = nullptr;
			if (blockEnabled == true)
			{
				manager->displayTunnelUsed(true);
			}
			else if (swapEnabled == true)
			{
				manager->displaySectionSwap(true);
			}
		}
		else
		{
			if (blockEnabled == true)
			{
				blockEnabled = false;
				currentlySelectedMapTunnel = nullptr;
				manager->displayTunnelUsed(false);
			}
			if (swapEnabled == true)
			{
				swapEnabled = false;
				currentlySelectedMapTunnel = nullptr;
				manager->displaySectionSwap(false);
			}
			manager->displaySectionUsed(true);
		}
	}

	void setBlockEnabled(bool toggle)
	{
		blockEnabled = toggle;
	}
	
	bool getBlockEnabled()
	{
		return blockEnabled;
	}

	void setSwapEnabled(bool toggle)
	{
		swapEnabled = toggle;
	}

	bool getSwapEnabled()
	{
		return swapEnabled;
	}



	ESpawnableObject getObjectToSpawn()
	{
		return objectToSpawn;
	}

	bool getMapSpawned()
	{
		return mapSpawned;
	}

	void spawnObject();

	void SetMapScale(float newScale);

	void RotateMap(float rotationDirection);

	void toggleMapLeftRotate(bool value)
	{
		mapLeftRotate = value;
	}

	void toggleMapRightRotate(bool value)
	{
		mapRightRotate = value;
	}

	void SpawnMap();

	void resetSelection() 
	{
		currentlySelectedMapSection = nullptr;
		currentlySelectedMapTunnel = nullptr;
		swapSelectedMapSection1 = nullptr;
		swapSelectedMapSection2 = nullptr;
		setObjectToSpawn(ESpawnableObject::None);
	}

	void isMapSetup();

	bool getMapSetup()
	{
		return mapSetup;
	}

	void blockTunnel();

	void swapObjects();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AR")
	UARSessionConfig* ARSessionConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AR")
	bool showPlanes = true;

	bool mapSpawned = false;

	UPROPERTY(EditAnywhere, Category = "AR")
	TSubclassOf<AActor> ARPlaneClass;

	TMap<UARPlaneGeometry*, AActor*> activePlanes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUIConnectionWidget> ConnectionWidgetBlueprintClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UARMapSetupUI> MapSetupWidgetBlueprintClass;

	UPROPERTY()
	UUIConnectionWidget* connectionWidget;

	UPROPERTY()
	UARMapSetupUI* mapSetupWidget;

	UPROPERTY()
	AARVRGameManager* manager;

	UPROPERTY()
	AMapSection* currentlySelectedMapSection;

	UPROPERTY()
	AMapTunnel* currentlySelectedMapTunnel;

	UPROPERTY()
	AMapSection* swapSelectedMapSection1;

	UPROPERTY()
	AMapSection* swapSelectedMapSection2;

private:	

	void OnScreenTouch();
	AActor* CreatePlaneActor(UARPlaneGeometry* planeGeometry);
	void DestroyOldPlanes();
	void UpdatePlanes();
	APlayerController* playerController;
	
	AActor* selectedPlane;
	UARPlaneGeometry* selectedPlaneGeometry;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* unselectedPlaneMaterial;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* selectedPlaneMaterial;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* selectedMapMaterial;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* unselectedMapMaterial;

	UPROPERTY(EditAnywhere, Category = "AR Plane")
	UMaterialInterface* blockedMapMaterial;

	void changeSelected(AActor* planeToChange);

	bool planeSelected = false;

	bool connectionTypeSelected = false;

	ESpawnableObject objectToSpawn;
	
	float currentMapScale = 1.0f;

	float baseScaleFactor = 1.0f;

	bool mapLeftRotate = false;

	bool mapRightRotate = false;

	bool mapSetup = false;

	bool blockEnabled = false;

	bool swapEnabled = false;
};
