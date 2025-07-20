// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPawn.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UIConnectionWidget.h"
#include "UIObserver.h"
#include "ARMapSetupUI.h"
#include "ARVRGameManager.h"
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
	objectToSpawn = AARVRGameManager::ESpawnableObject::None;
}

// Called when the game starts or when spawned
void AARPawn::BeginPlay()
{
	Super::BeginPlay();

	AARVRGameManager* Manager = Cast<AARVRGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AARVRGameManager::StaticClass()));
	if (!Manager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find Manager!"));
	}
	manager = Manager;

	UWorld* currentWorld = GetWorld();
	playerController = UGameplayStatics::GetPlayerController(currentWorld, 0);

	connectionWidget = CreateWidget<UUIConnectionWidget>(playerController, ConnectionWidgetBlueprintClass);
	connectionWidget->setupUIObserver(this);
	connectionWidget->AddToViewport();	

	mapSetupWidget = CreateWidget<UARMapSetupUI>(playerController, MapSetupWidgetBlueprintClass);
	mapSetupWidget->setupUIObserver(this);

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
				else {
					if (!manager->vrPlayerTurn) {
						if (hitActor)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Hit Actor: %s"), *hitActor->GetName()));

							if (hitActor->IsA(AMapSection::StaticClass()) && objectToSpawn != AARVRGameManager::ESpawnableObject::None) {
								AMapSection* selectedMapSection = Cast<AMapSection>(hitActor);
								if (currentlySelectedMapSection == selectedMapSection) {
									currentlySelectedMapSection->swapSelectedMaterial();
								}
								if (!currentlySelectedMapSection) {
									selectedMapSection->swapSelectedMaterial();
									currentlySelectedMapSection = selectedMapSection;
								}
								if(currentlySelectedMapSection != selectedMapSection){
									currentlySelectedMapSection->swapSelectedMaterial();
									selectedMapSection->swapSelectedMaterial();
									currentlySelectedMapSection = selectedMapSection;
								}
								if (mapSetupWidget->getConfirmButton()->GetVisibility() == ESlateVisibility::Hidden)
								{
									mapSetupWidget->changeButtonVisibility(mapSetupWidget->getConfirmButton());
								}
							}
							else {
								currentlySelectedMapSection = nullptr;
							}
						}
					}
				}
			}

	}
}

void AARPawn::SpawnMap()
{

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
			if (child->Tags.Contains("ARMove"))
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
	command->commandType = AARVRGameManager::EMessageType::ARPlayerSelection;
	command->sequenceCount = manager->getSequenceCount();
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
				// Only show planes that are being tracked
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

UUserWidget* AARPawn::getConnectionWidget() const
{
	return connectionWidget;
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
	manager->spawnEntityAtSection(currentlySelectedMapSection, objectToSpawn);
	currentlySelectedMapSection->swapSelectedMaterial();
	currentlySelectedMapSection = nullptr;
	objectToSpawn = AARVRGameManager::ESpawnableObject::None;
	manager->switchTurns();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed")));
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

	FTimerHandle DelayPlanesHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayPlanesHandle, [this]()
		{
			showPlanes = true;
		}, 0.5f, false);  // Delay allows AR system to stabilize
}