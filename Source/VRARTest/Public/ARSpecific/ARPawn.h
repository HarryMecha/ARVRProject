// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARBlueprintLibrary.h"
#include "ARSessionConfig.h"
#include "Map/MapSection.h"
#include "ARVRGameManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"

class UUIObserver;
class UUIConnectionWidget;
class UARMapSetupUI;

#include "ARPawn.generated.h"

UENUM(BlueprintType)
enum class EInteractionMode : uint8
{
	None,
	SpawningObject,
	BlockingTunnel,
	SwappingObjects,
	ApplyingFrenzy,
	ZoomingIn,
	ZoomingOut
};

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

	// Getter method for the Connection Widget that diplays the connection UI.
	UUIConnectionWidget* getConnectionWidget() const;

	// Getter method for the Map Setup Widget that displays the player options during gameplay.
	UARMapSetupUI* getMapSetupWidget() const;

	// resetARState() will call the DestroyOldPlanes function in order to empty the array of mapPlane actors will reset the currently selected plane and all booleans associated, will hide
	// the mapRoot actor that holds the map that has currently been anchored to that plane aswell as all it's children, and hide the confirm button from view. This will allow new map planes
	// to be spawned for selection. It is called from the reset button.
	void resetARState();

	// ConfirmMapChoice() will be called once a plane has been successfully selected for the map to be spawned on all other map planes actors will de destroyed via DestroyOldPlanes() and a
	// arPlayerSelectionCommand will be sent over the network to notify the other player that the AR Player has spawned in the map, switching the arPlayerPlaneSelected bool to true on their
	// game instance allowing certain commands to now run. The plane selection UI is hidden from the player and the mapSetupWidget is now shown.
	void ConfirmMapChoice();

	// startARSession() starts the ARSession using the ARSessionConfig as a parameter
	void startARSession();
	
	// stopARSession() stops the ARSession 
	void stopARSession();

	/*
		setInteractionMode(EInteractionMode newMode, ESpawnableObject object = ESpawnableObject::None), is used to replace the current interaction with a new one, these interaction modes 
		represent which current button the AR user has selected in order to spawn a specific object or cast a specific speel, this will also turn off any visual elements from the previous
		interaction mode that may have still been present.
	*/
	void setInteractionMode(EInteractionMode newMode, ESpawnableObject object = ESpawnableObject::None);

	//  setter method for the objectToSpawn variable
	void setObjectToSpawn(ESpawnableObject objectType);
	
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

	void setFrenzyEnabled(bool toggle)
	{
		frenzyEnabled = toggle;
	}

	bool getFrenzyEnabled()
	{
		return frenzyEnabled;
	}

	ESpawnableObject getObjectToSpawn()
	{
		return objectToSpawn;
	}

	bool getMapSpawned()
	{
		return mapSpawned;
	}

	// spawnObject() will call the spawnEntityAtSection() function in the ARVRManager class, this will spawn an object specified by the objectToSpawn variable
	// in the section defined by the currentlySelectedMapSection, after calling this function the interaction mode and objectToSpawn are reset closing any associated
	// UI elements allowing new objects and sections to be selected.
	void spawnObject();

	// SetMapScale() takes in an input from the slider widget object and increases the Actor3DScale of the map.
	void SetMapScale(float newScale);

	// SetMapScale() takes in an input from the two rotation buttons objects and rotates the map actor.
	void RotateMap(float rotationDirection);

	void toggleMapLeftRotate(bool value)
	{
		mapLeftRotate = value;
	}

	void toggleMapRightRotate(bool value)
	{
		mapRightRotate = value;
	}

	/* 
	 SpawnMap() will take the value of the curently selected plane gerometry calulcate it's width and depth, the same is done with the boxCollider that encompasses the map
	 these two figures are divided agsint each other in order to generate a scale factor that he map root will be multiplied by in order to accurately fit on top of the plane,
	 this can be changed by the user using the slider, the map in the placed in the center of the plane. All the children of the mapRoot will be set to visible if they were previously
	 hidden by the ARVRManager on BeginPlay().
	*/
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

	// blockTunnel() is called if the player has selected both the block tunnel spell and a MapTunnel object using the touch screen, it will set that actor to blocked visually turning it
	// red and sending a blockedWall command over the network so this state can be replicated by the other players game.
	void blockTunnel();

	// swapObjects() is called if the player has selected both the swap objects spell and two different MapSection objects that contain entities using the touch screen, it will obtain the 
	// spawn position of both of the map sections and swap their pointers to the currently occupied entity around with each other, so the change is reflected in the code. A SwapSection Command
	// is also sent to the other player so they can replicate the change in their game.
	void swapObjects();

	// swapObjects() is called if the player has selected both the frenzy spell and a MapSection objects that contain entity using the touch screen, it will obtain the LivingEntity from the map section 
	// and apply Frenzy to it. A ApplyFrenzy Command is also sent to the other player so they can replicate the change in their game.
	void applyFrenzy();

	EInteractionMode currentInteractionMode = EInteractionMode::None;

	AMapSection* getCurrentlySelectedMapSection()
	{
		return currentlySelectedMapSection;
	}

	bool getIsZoomedIn()
	{
		return isZoomedIn;
	}


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AR")
	UARSessionConfig* ARSessionConfig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* cameraComponent;

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
	AMapSection* focusedMapSection;

	UPROPERTY()
	AMapTunnel* currentlySelectedMapTunnel;

	UPROPERTY()
	AMapSection* swapSelectedMapSection1;

	UPROPERTY()
	AMapSection* swapSelectedMapSection2;

	void setZoomedIn(bool toggle)
	{
		isZoomedIn = toggle;
	}

	bool getZoomedIn()
	{
		return isZoomedIn;
	}

	// The zoomIntoSection() funciton allows the player to zoom in to specific map sections, it will take the location the mapRoot and the selected map section and subtract them together
	// in order to determine the offset that needs to be applied in order to keep the selected map section in the center of the screen, this is then multiplied by a zoom multiplier and the
	// position of the camera when the same zoom mutliplier is applied in order to position and scale the map in the center of the camera viewpoint at an enlarged size. This does not move
	// the position of the camera however only the map as it compromises the AR tracking
	void zoomIntoSection(AMapSection* sectionToZoom);

	// zoomOutSection() undoes the effects of the zoomInSection returning the maps position and scale to before the map was zoomed in.
	void zoomOutSection();

private:	

	// OnScreenTouch() handles when events when the screen is touched, the player controller will deproject the world, essentially creating a 2D representation of what is seen through the camera actor,
	// a line is cast from the point of touch on this representation and if it is successful in hitting an actor it will be stored. The different funcitonalities of what to do with this hit actor are 
	// delegated to other functions.
	void OnScreenTouch();
	
	// CreatePlaneActor() takes the planeGeometry obtain via the OnScreenTouch() hitting a ARPlaneGeometry, it will then spawn a plane actor and scale it to the extents of the geometery, returning it
	// so it can be added to the activePlanes array that keeps track of all the plane actors currently on the screen.
	AActor* CreatePlaneActor(UARPlaneGeometry* planeGeometry);
	
	// DestroyOldPlanes() Clears the activePlanes array, destorying all the planes and geometry within allowing new planes to be spawned.
	void DestroyOldPlanes();
	
	// UpdatePlanes() will check whether planeGeometries are detected by the ARSession are currently in the activePlanes array if they aren't then it will add them and spawn in a plane actor in order to
	// visualise them and facilitate the touching of them using the touch screen.
	void UpdatePlanes();
	
	APlayerController* playerController;
	
	// OnScreenTouchNone() function handles screen touch logic when no AR user selection has been made, handling the funtionality for zooming, it will first reset the visuals of the map and the currently
	// selected map section and then when the screen is pressed will check whether the section they have clicked is already the currentlySelectedMapSection indicating they want to deselect it, or it is not
	// meaning they want to select it, swapping the material of the section and assigning or unassigning it accordingly.
	void OnScreenTouchNone(AMapSection* selectedSection);

	// OnScreenTouchSpawn() function handles screen touch logic when the AR user has selected a spawnEntity button, it will first reset the visuals of the map and the currently selected map section and 
	// then when the screen is pressed will check whether the section they have clicked is already the currentlySelectedMapSection indicating they want to deselect it, or it is not meaning they want to 
	// select it, swapping the material of the section and assigning or unassigning it accordingly.
	void OnScreenTouchSpawn(AMapSection* selectedSection);

	// OnScreenTouchSpawn() function handles screen touch logic when the AR user has selected the block tunnel button, it will first reset the visuals of the map and the currently selected map tunnels and 
	// sections. When the screen is pressed will check whether the tunnel they have clicked is already the currentlySelectedMapTunnel indicating they want to deselect it, or it is not meaning they want to 
	// select it, swapping the material of the section and assigning or unassigning it accordingly.
	void OnScreenTouchBlock(AMapTunnel* selectedTunnel);

	// OnScreenTouchSwap() function handles screen touch logic when the AR user has selected the swap entities button, it will first reset the visuals of the map and the currentlyselected map section 
	// and then when the screen is pressed will check whether the section they have clicked is already one of the two selected planes needed for the swap indicating they want to deselect it or it is not
	// meaning they want to select it, swapping the material of the section and assigning or unassigning it accordingly.
	void OnScreenTouchSwap(AMapSection* selectedSection);

	// OnScreenTouchFrenzy() function handles screen touch logic when the AR user has selected the frenzy button it will first reset the visuals of the map and the currently
	// selected map section and then when the screen is pressed will check whether the section they have clicked is already the currentlySelectedMapSection indicating they want to deselect it or it is not
	// meaning they want to select it, swapping the material of the section and assigning or unassigning it accordingly.
	void OnScreenTouchFrenzy(AMapSection* selectedSection);

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

	bool frenzyEnabled = false;

	bool isZoomedIn;

	FVector zoomedInLocation;

	FVector originalLocation;

	FVector originalScale;

	float zoomMultiplier = 3.5f;

	float zoomMaxDistance = 50.0;
};
