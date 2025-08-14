// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UIConnectionWidget.h"
#include "UIObserver.h"
#include "ARMapSetupUI.h"
#include "ARVRGameManager.h"
#include "LivingPooledEntity.h"
#include "PooledEntityInterface.h"
#include "Components/BoxComponent.h"
#include "Command.h"
#include <iostream>

// Sets default values
AARPawn::AARPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	selectedPlane = nullptr;
	manager = nullptr;
	currentlySelectedMapSection = nullptr;
	currentlySelectedMapTunnel = nullptr;
	objectToSpawn = ESpawnableObject::None;
}

// Called when the game starts or when spawned
void AARPawn::BeginPlay()
{
	Super::BeginPlay();

	cameraComponent = FindComponentByClass<UCameraComponent>();

	AARVRGameManager* Manager = Cast<AARVRGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AARVRGameManager::StaticClass()));
	if (!Manager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find Manager!"));
	}
	manager = Manager;

	UWorld* currentWorld = GetWorld();
	playerController = UGameplayStatics::GetPlayerController(currentWorld, 0);

	connectionWidget = CreateWidget<UUIConnectionWidget>(playerController, ConnectionWidgetBlueprintClass);
	if (connectionWidget)
	{
		connectionWidget->setupUIObserver(this);
		connectionWidget->AddToViewport();
	}

	mapSetupWidget = CreateWidget<UARMapSetupUI>(playerController, MapSetupWidgetBlueprintClass);
	if (mapSetupWidget)
	{
		mapSetupWidget->setupUIObserver(this);
		mapSetupWidget->setPopUpText("Place down all the Chests and Traps by selecting map sections");
	}

	mapLeftRotate = false;

	mapRightRotate = false;
	
	
}

// Called every frame
void AARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (showPlanes && connectionTypeSelected)
	{
		UpdatePlanes();
	}
	if (mapSpawned)
	{
		if (mapLeftRotate)
		{
			RotateMap(-1.0f);
		}
		else if (mapRightRotate)
		{
			RotateMap(1.0f);
		}
	}

}

// Called to bind functionality to input
void AARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Tap", IE_Pressed, this, &AARPawn::OnScreenTouch);

}

void AARPawn::OnScreenTouch()
{
	//The coordinate of the players touch and a bool for if the screen has been pressed.
	FVector2D touchCoord;
	bool screenPressed;
	//This will set the position of the screen press to the X and Y variables and set the bool to true.
	playerController->GetInputTouchState(ETouchIndex::Touch1, touchCoord.X, touchCoord.Y, screenPressed);

	FVector worldLocation, worldDirection;
	if (playerController->DeprojectScreenPositionToWorld(touchCoord.X, touchCoord.Y, worldLocation, worldDirection))
	{
		FVector traceStart = worldLocation;
		FVector traceEnd = worldLocation + (worldDirection * 100000.0f);

		FHitResult hitResult;
		FCollisionQueryParams traceParams;
		traceParams.bReturnPhysicalMaterial = false;

		bool hit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, traceParams);

		if (hit)
		{
			AActor* hitActor = hitResult.GetActor();
			if (!mapSpawned) {
				changeSelected(hitActor);
			}
			else
			{
				if (manager->getCurrentTurn() == EPlayerRole::AR)
				{
					if (hitActor)
					{
						switch (currentInteractionMode)
						{
						case EInteractionMode::SpawningObject:
							if (hitActor->IsA(AMapSection::StaticClass())) 
							{
								AMapSection* selectedMapSection = Cast<AMapSection>(hitActor);
								OnScreenTouchSpawn(selectedMapSection);
							}
							break;

						case EInteractionMode::BlockingTunnel:
							if (hitActor->IsA(AMapTunnel::StaticClass()))
							{
								AMapTunnel* selectedMapTunnel = Cast<AMapTunnel>(hitActor);
								OnScreenTouchBlock(selectedMapTunnel);
							}
							break;

						case EInteractionMode::SwappingObjects:
							if (hitActor->IsA(AMapSection::StaticClass()))
							{
								AMapSection* selectedMapSection = Cast<AMapSection>(hitActor);
								OnScreenTouchSwap(selectedMapSection);
							}
							break;

						case EInteractionMode::ApplyingFrenzy:
							if (hitActor->IsA(AMapSection::StaticClass()))
							{
								AMapSection* selectedMapSection = Cast<AMapSection>(hitActor);
								OnScreenTouchFrenzy(selectedMapSection);
							}
							break;

						case EInteractionMode::None:
							if (hitActor->IsA(AMapSection::StaticClass()))
							{
								AMapSection* selectedMapSection = Cast<AMapSection>(hitActor);
								OnScreenTouchNone(selectedMapSection);
							}
							break;
						}
					}
					else
					{

					}
				}
			}
		}
	}
}

void AARPawn::OnScreenTouchNone(AMapSection* selectedMapSection)
{
	if (selectedMapSection->getSpecialSection() == false && isZoomedIn == false)
	{
		// Check if clicking the same section that's already selected (deselect it)
		if (currentlySelectedMapSection == selectedMapSection)
		{
			currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
			currentlySelectedMapSection = nullptr;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
		}
		// Selecting a new section
		else
		{
			// Clear previous selection if any
			if (currentlySelectedMapSection)
			{
				currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
			}

			// Select the new section
			selectedMapSection->swapSelectedMaterial(selectedMapMaterial);
			currentlySelectedMapSection = selectedMapSection;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), true);
		}
	}
}

void AARPawn::OnScreenTouchSpawn(AMapSection* selectedMapSection)
{

	if (selectedMapSection->getSpecialSection() == false && selectedMapSection->getSectionUsed() == false) {

		if (currentlySelectedMapSection) {
			currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
			currentlySelectedMapSection = nullptr;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
		}
		if (currentlySelectedMapSection == selectedMapSection) {
			currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
			currentlySelectedMapSection = nullptr;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
		}
		else
		{
			if (!currentlySelectedMapSection)
			{
				selectedMapSection->swapSelectedMaterial(selectedMapMaterial);
				currentlySelectedMapSection = selectedMapSection;
			}
			else if (currentlySelectedMapSection != selectedMapSection)
			{
				currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
				selectedMapSection->swapSelectedMaterial(selectedMapMaterial);
				currentlySelectedMapSection = selectedMapSection;
			}
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), true);
		}
	}
}


void AARPawn::OnScreenTouchBlock(AMapTunnel* selectedMapTunnel)
{
	if (currentlySelectedMapSection) {
		currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
		currentlySelectedMapSection = nullptr;
		mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
		mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
	}

	if (selectedMapTunnel->getTunnelBlocked() == false)
	{
		if (currentlySelectedMapTunnel == selectedMapTunnel)
		{
			currentlySelectedMapTunnel->swapSelectedMaterial(unselectedMapMaterial);
			currentlySelectedMapTunnel = nullptr;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
		}
		else
		{
			if (!currentlySelectedMapTunnel)
			{
				selectedMapTunnel->swapSelectedMaterial(selectedMapMaterial);
				currentlySelectedMapTunnel = selectedMapTunnel;
			}
			else if (currentlySelectedMapTunnel != selectedMapTunnel)
			{
				currentlySelectedMapTunnel->swapSelectedMaterial(unselectedMapMaterial);
				selectedMapTunnel->swapSelectedMaterial(selectedMapMaterial);
				currentlySelectedMapTunnel = selectedMapTunnel;
			}
				mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), true);
		}
	}
}


void AARPawn::OnScreenTouchSwap(AMapSection* selectedMapSection)
{
	if (selectedMapSection->getSpecialSection() == false && selectedMapSection->getSectionUsed() == true) 
	{
		if (currentlySelectedMapSection)
		{
			currentlySelectedMapTunnel->swapSelectedMaterial(unselectedMapMaterial);
			currentlySelectedMapTunnel = nullptr;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);

		}
		if (selectedMapSection->getSectionUsed() == true)
		{
			if (selectedMapSection == swapSelectedMapSection1)
			{
				swapSelectedMapSection1->swapSelectedMaterial(unselectedMapMaterial);
				swapSelectedMapSection1 = nullptr;
			}
			else if (selectedMapSection == swapSelectedMapSection2)
			{
				swapSelectedMapSection2->swapSelectedMaterial(unselectedMapMaterial);
				swapSelectedMapSection2 = nullptr;
			}
			else if (!swapSelectedMapSection1)
			{
				swapSelectedMapSection1 = selectedMapSection;
				swapSelectedMapSection1->swapSelectedMaterial(selectedMapMaterial);
			}
			else if (!swapSelectedMapSection2)
			{
				swapSelectedMapSection2 = selectedMapSection;
				swapSelectedMapSection2->swapSelectedMaterial(selectedMapMaterial);
			}
			if (swapSelectedMapSection1 && swapSelectedMapSection2)
			{
				mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), true);
			}
			else
			{
				mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);

			}
		}
	}
}


void AARPawn::OnScreenTouchFrenzy(AMapSection* selectedMapSection)
{
	if (selectedMapSection->getSpecialSection() == false && selectedMapSection->getSectionUsed() == true)
	{
		if (currentlySelectedMapSection)
		{
			currentlySelectedMapTunnel->swapSelectedMaterial(unselectedMapMaterial);
			currentlySelectedMapTunnel = nullptr;
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
			mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);

		}
		AActor* currentEntity = selectedMapSection->getCurrentEntity();
		if (currentEntity && currentEntity->IsA(ALivingPooledEntity::StaticClass()))
		{
			ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(currentEntity);
			if (currentlySelectedMapSection) 
			{
				currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
				currentlySelectedMapSection = nullptr;
				mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
			}
			if (livingEntity->getIsFrenzied() == false)
			{
				if (currentlySelectedMapSection == selectedMapSection)
				{
					currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
					currentlySelectedMapSection = nullptr;
					mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
				}
				else
				{
					if (!currentlySelectedMapSection)
					{
						selectedMapSection->swapSelectedMaterial(selectedMapMaterial);
						currentlySelectedMapSection = selectedMapSection;
					}
					else if (currentlySelectedMapSection != selectedMapSection)
					{
						currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
						selectedMapSection->swapSelectedMaterial(selectedMapMaterial);
						currentlySelectedMapSection = selectedMapSection;
					}
					mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), true);
				}
			}
		}
	}
}


void AARPawn::SpawnMap()
{

	if (selectedPlane)
	{
		FString planeName = selectedPlane->GetName();
		FString planeLocationStr = selectedPlane->GetActorLocation().ToString();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Spawning map on plane: %s at location: %s"), *planeName, *planeLocationStr));
	}

	FTransform planeTransform = selectedPlane->GetTransform();	
	
	AActor* mapRoot = manager->GetMapRoot();

	UBoxComponent* boundsBox = mapRoot->FindComponentByClass<UBoxComponent>();
	FVector boxExtent = boundsBox->GetScaledBoxExtent();
	float mapWidth = boxExtent.X * 2.0f;
	float mapDepth = boxExtent.Y * 2.0f;

	FVector planeExtent = selectedPlaneGeometry->GetExtent();
	float planeWidth = planeExtent.X * 2.0f;
	float planeDepth = planeExtent.Y * 2.0f;

	baseScaleFactor = FMath::Min(planeWidth / mapWidth, planeDepth / mapDepth);
	baseScaleFactor = FMath::Clamp(baseScaleFactor, 0.05f, 2.0f);  // Adjust bounds as needed
	currentMapScale = 1.0f;


	if (mapRoot)
	{
		FVector planeLocation = selectedPlane->GetActorLocation();
		FRotator planeRotation = selectedPlane->GetActorRotation();

		mapRoot->SetActorLocation(planeLocation);
		mapRoot->SetActorRotation(planeRotation);
		mapRoot->SetActorScale3D(FVector(baseScaleFactor * currentMapScale));

		TArray<AActor*> mapRootChildren;
		mapRoot->GetAttachedActors(mapRootChildren);
		for (AActor* child : mapRootChildren)
		{
			if (child->Tags.Contains("ARMove") && !child->Tags.Contains("VRRep"))
			{
				child->SetActorHiddenInGame(false);
			}

		}
	}
	mapSpawned = true;
	DestroyOldPlanes();
	showPlanes = false;
	connectionWidget->changeWidgetVisibility(connectionWidget->getScaleSlider());
	connectionWidget->changeWidgetVisibility(connectionWidget->getLeftRotateButton());
	connectionWidget->changeWidgetVisibility(connectionWidget->getRightRotateButton());
	connectionWidget->changeWidgetVisibility(connectionWidget->getResetButton());
}

void AARPawn::ConfirmMapChoice() 
{
	mapSpawned = true;
	DestroyOldPlanes();
	showPlanes = false;
	TSharedPtr<arPlayerSelectionCommand> command = MakeShared<arPlayerSelectionCommand>();
	command->commandType = EMessageType::ARPlayerSelection;
	command->sequenceCount = manager->getNextSequenceCount();
	manager->arPlayerPlaneSelected = true;
	manager->clearIncomingQueue();
	manager->clearOutgoingQueue();
	manager->AddToOutgoingCommandQueue(command);
	connectionWidget->RemoveFromViewport();
	mapSetupWidget->AddToViewport();
}

AActor* AARPawn::CreatePlaneActor(UARPlaneGeometry* planeGeometry)
{
	FTransform planeTransform = planeGeometry->GetLocalToWorldTransform();
	FVector planeBounds = planeGeometry->GetExtent();

	AActor* spawnedPlane = GetWorld()->SpawnActor<AActor>(ARPlaneClass, planeTransform.GetLocation(), planeTransform.GetRotation().Rotator());
	AActor* parentActor = spawnedPlane->GetAttachParentActor();

	FVector defaultMeshSize = FVector(100.f, 100.f, 1.f);
	FVector newPlaneScale = FVector(planeBounds.X / (defaultMeshSize.X * 0.5f), planeBounds.Y / (defaultMeshSize.Y * 0.5f), 1.0f);

	UStaticMeshComponent* planeMesh = spawnedPlane->FindComponentByClass<UStaticMeshComponent>();

	planeMesh->SetWorldScale3D(newPlaneScale);
	return spawnedPlane;

}

void AARPawn::DestroyOldPlanes()
{
	for (auto& planePair : activePlanes)
	{
		AActor* planeActor = planePair.Value; 
        if (planeActor)
        {
            planeActor->Destroy();
        }
    }
	activePlanes.Empty();
}

void AARPawn::UpdatePlanes()
{
	TArray<UARTrackedGeometry*> trackedGeometries = UARBlueprintLibrary::GetAllGeometries();

	for (UARTrackedGeometry* geometry : trackedGeometries)
	{
			if (UARPlaneGeometry* planeGeometry = Cast<UARPlaneGeometry>(geometry))
			{
				if (planeGeometry->GetTrackingState() == EARTrackingState::Tracking)
				{
					if (!activePlanes.Contains(planeGeometry))
					{
						activePlanes.Add(planeGeometry, CreatePlaneActor(planeGeometry));
					}
				}
			}

	}
}

UUIConnectionWidget* AARPawn::getConnectionWidget() const
{
	return connectionWidget;
}

UARMapSetupUI* AARPawn::getMapSetupWidget() const
{
	return mapSetupWidget;
}

void AARPawn::changeSelected(AActor* planeToChange)
{
	UARPlaneGeometry* planeGeometry = nullptr;
	planeSelected = true;
	AActor* planeActor = planeToChange;
	for (auto& it : activePlanes) {
		if (it.Value == planeActor) {
			planeGeometry = it.Key;
		}
	}
	if(!planeGeometry)
	{
		return;
	}

	if (!selectedPlane)
	{
		connectionWidget->changeWidgetVisibility(connectionWidget->getConfirmButton());
		UStaticMeshComponent* planeMesh = planeActor->FindComponentByClass<UStaticMeshComponent>();
		planeMesh->SetMaterial(0, selectedPlaneMaterial);
		selectedPlane = planeActor;
		selectedPlaneGeometry = planeGeometry;

	}
	else if(selectedPlane == planeActor)
	{
		connectionWidget->changeWidgetVisibility(connectionWidget->getConfirmButton());
		UStaticMeshComponent* planeMesh = planeActor->FindComponentByClass<UStaticMeshComponent>();
		planeMesh->SetMaterial(0, unselectedPlaneMaterial);
		selectedPlane = nullptr;
		selectedPlaneGeometry = nullptr;
		planeSelected = false;
	}
	else 
	{
		
		UStaticMeshComponent* planeMesh = planeActor->FindComponentByClass<UStaticMeshComponent>();
		planeMesh->SetMaterial(0, selectedPlaneMaterial);
		planeMesh = selectedPlane->FindComponentByClass<UStaticMeshComponent>();
		planeMesh->SetMaterial(0, unselectedPlaneMaterial);
		selectedPlane = planeActor;
		selectedPlaneGeometry = planeGeometry;

	}
}

void AARPawn::startARSession()
{
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Starting AR Session"));
	UARBlueprintLibrary::StartARSession(ARSessionConfig);
	if (UARBlueprintLibrary::GetARSessionStatus().Status == EARSessionStatus::Running)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("AR Session already running!"));
		return;
	}
	connectionTypeSelected = true;
}

void AARPawn::spawnObject()
{
	if (mapSetup == false) {
		isMapSetup();
	}
	if (currentlySelectedMapSection != nullptr)
	{
		manager->spawnEntityAtSection(currentlySelectedMapSection, objectToSpawn);
		currentlySelectedMapSection->swapSelectedMaterial(unselectedMapMaterial);
		mapSetupWidget->resetObjectType();
		setInteractionMode(EInteractionMode::None);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("spawnObject failed: currentlySelectedMapSection was null!"));
	}
}

void AARPawn::SetMapScale(float newScale)
{
	AActor* mapRoot = manager->GetMapRoot();
	if (mapRoot)
	{
		currentMapScale = newScale;
		FVector newScaleVector = FVector(baseScaleFactor * newScale);
		mapRoot->SetActorScale3D(newScaleVector);
	}
}

void AARPawn::RotateMap(float rotationDirection)
{
	AActor* mapRoot = manager->GetMapRoot();
	if (mapRoot)
	{

		FRotator currentRotation = mapRoot->GetActorRotation();

		float rotationSpeed = 25.0f;
		float deltaTime = GetWorld()->GetDeltaSeconds();
		float rotationDelta = rotationDirection * rotationSpeed * deltaTime;

		FRotator newRotation = currentRotation + FRotator(0, rotationDelta, 0);
		mapRoot->SetActorRotation(newRotation);
	}
}

void AARPawn::resetARState()
{
	DestroyOldPlanes();

	// Reset map and selection
	selectedPlane = nullptr;
	selectedPlaneGeometry = nullptr;
	planeSelected = false;
	mapSpawned = false;

	AActor* mapRoot = manager->GetMapRoot();
	if (mapRoot)
	{
		mapRoot->SetActorScale3D(FVector(baseScaleFactor * currentMapScale));
		mapRoot->SetActorHiddenInGame(true);

		TArray<AActor*> mapRootChildren;
		mapRoot->GetAttachedActors(mapRootChildren);
		for (AActor* child : mapRootChildren)
		{
			if (child->Tags.Contains("ARMove"))
			{
				child->SetActorHiddenInGame(true);
			}
		}
	}

	if (connectionWidget)
	{
		connectionWidget->changeWidgetVisibility(connectionWidget->getConfirmButton());
		connectionWidget->getScaleSlider()->SetValue(1.0f);
		connectionWidget->changeWidgetVisibility(connectionWidget->getScaleSlider());
		connectionWidget->changeWidgetVisibility(connectionWidget->getLeftRotateButton());
		connectionWidget->changeWidgetVisibility(connectionWidget->getRightRotateButton());
		connectionWidget->changeWidgetVisibility(connectionWidget->getResetButton());
	}
}

void AARPawn::isMapSetup()
{
	if (mapSetupWidget->getTreasureCount() == 0 && mapSetupWidget->getTrapCount() == 0)
	{
		mapSetup = true;
		mapSetupWidget->switchViews();
	}
	else
	{
		mapSetup = false;
	}
}

void AARPawn::blockTunnel()
{
	if (currentlySelectedMapTunnel) {
		currentlySelectedMapTunnel->setTunnelBlocked();
		manager->sendBlockedWallCommand(currentlySelectedMapTunnel);
		currentlySelectedMapTunnel->swapSelectedMaterial(blockedMapMaterial);
		blockEnabled = false;
		mapSetupWidget->resetObjectType();
		setInteractionMode(EInteractionMode::None);
		manager->displayTunnelUsed(false);
	}
}

void AARPawn::swapObjects()
{
	if (swapSelectedMapSection1 && swapSelectedMapSection2)
	{
		AActor* actorToSwap1 = swapSelectedMapSection1->getCurrentEntity();
		AActor* actorToSwap2 = swapSelectedMapSection2->getCurrentEntity();
		if (actorToSwap1)
		{
			UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(actorToSwap1->GetComponentByClass(UPooledEntityComponent::StaticClass()));

			if (pooledComponent)
			{
				pooledComponent->setOwnerSection(swapSelectedMapSection2);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find PooledComponent!"));

			}
		}

		if (actorToSwap2)
		{
			UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(actorToSwap2->GetComponentByClass(UPooledEntityComponent::StaticClass()));

			if (pooledComponent)
			{
				pooledComponent->setOwnerSection(swapSelectedMapSection1);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find PooledComponent!"));
			}
		}
		swapSelectedMapSection1->spawnActorAtPoint(actorToSwap2);
		swapSelectedMapSection1->setCurrentEntity(actorToSwap2);
		swapSelectedMapSection2->spawnActorAtPoint(actorToSwap1);
		swapSelectedMapSection2->setCurrentEntity(actorToSwap1);
		manager->sendSwapSectionCommand(swapSelectedMapSection1, swapSelectedMapSection2);
		swapEnabled = false;
		mapSetupWidget->resetObjectType();
		setInteractionMode(EInteractionMode::None);
		manager->displaySectionSwap(false);
	}
}

void AARPawn::applyFrenzy()
{
	if (currentlySelectedMapSection)
	{
		AActor* entity = currentlySelectedMapSection->getCurrentEntity();
		if (entity->IsA(ALivingPooledEntity::StaticClass()))
		{
			ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(entity);
			livingEntity->toggleFrenzyAR(true);
			manager->sendApplyFrenzyCommand(currentlySelectedMapSection);
		}
		frenzyEnabled = false;
		mapSetupWidget->resetObjectType();
		setInteractionMode(EInteractionMode::None);
		manager->displaySectionFrenzy(false);
	}
}

void AARPawn::zoomIntoSection(AMapSection* sectionToZoom)
{
	focusedMapSection = sectionToZoom;
	isZoomedIn = true;

	AActor* mapRoot = manager->GetMapRoot();

	originalLocation = mapRoot->GetActorLocation();
	originalScale = mapRoot->GetActorScale3D();

	FVector newMapScale = originalScale * zoomMultiplier;

	FVector sectionLocation = sectionToZoom->GetActorLocation();
	FVector focalPoint = cameraComponent->GetComponentLocation() + (cameraComponent->GetForwardVector() * zoomMaxDistance);
	
	FVector offsetFromMapRootToSection = sectionLocation - originalLocation;
	FVector scaledOffset = offsetFromMapRootToSection * zoomMultiplier;
	FVector newMapLocation = focalPoint - scaledOffset;

	mapRoot->SetActorLocation(newMapLocation);
	mapRoot->SetActorScale3D(newMapScale);
	mapSetupWidget->getZoomButtonTitle()->SetText(FText::FromString("Zoom Out"));
}

void AARPawn::zoomOutSection()
{
	isZoomedIn = false;
	AActor* mapRoot = manager->GetMapRoot();
	mapRoot->SetActorLocation(originalLocation);
	mapRoot->SetActorScale3D(originalScale);

	focusedMapSection = nullptr;
	mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
	mapSetupWidget->getZoomButtonTitle()->SetText(FText::FromString("Zoom In"));

	currentInteractionMode = EInteractionMode::None;
}

void AARPawn::setObjectToSpawn(ESpawnableObject objectType)
{
	objectToSpawn = objectType;

	if (objectToSpawn != ESpawnableObject::None)
	{
		manager->displaySectionUsed(true);
	}
	else
	{
		manager->displaySectionUsed(false);
	}
}

void AARPawn::setInteractionMode(EInteractionMode newMode, ESpawnableObject object)
{
	setBlockEnabled(false);
	setSwapEnabled(false);
	setFrenzyEnabled(false);
	manager->displayTunnelUsed(false);
	manager->displaySectionSwap(false);
	manager->displaySectionFrenzy(false);
	mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton(), false);
	setObjectToSpawn(ESpawnableObject::None);
	currentInteractionMode = newMode;
	
	switch (newMode)
	{
	case EInteractionMode::None:
		break;

	case EInteractionMode::SpawningObject:
		mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
		setObjectToSpawn(object);
		break;

	case EInteractionMode::BlockingTunnel:
		mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
		setBlockEnabled(true);
		manager->displayTunnelUsed(true);
		break;

	case EInteractionMode::SwappingObjects:
		mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
		setSwapEnabled(true);
		manager->displaySectionSwap(true);
		break;

	case EInteractionMode::ApplyingFrenzy:
		mapSetupWidget->changeButtonVisibility(mapSetupWidget->getZoomButton(), false);
		setFrenzyEnabled(true);
		manager->displaySectionFrenzy(true);
		break;
	case EInteractionMode::ZoomingIn:
		if (isZoomedIn == false)
		{
			zoomIntoSection(currentlySelectedMapSection);
		}
		else if (isZoomedIn == true)
		{
			zoomOutSection();
		}
		return;
	}

	currentInteractionMode = newMode;
}

