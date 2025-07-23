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
		objectToSpawn = ESpawnableObject::None;
	}


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

	UUIConnectionWidget* connectionWidget;

	UARMapSetupUI* mapSetupWidget;

	AARVRGameManager* manager;

	AMapSection* currentlySelectedMapSection;

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

	void changeSelected(AActor* planeToChange);

	bool planeSelected = false;

	bool connectionTypeSelected = false;

	ESpawnableObject objectToSpawn;
	
	float currentMapScale = 1.0f;

	float baseScaleFactor = 1.0f;

	bool mapLeftRotate = false;

	bool mapRightRotate = false;

};
