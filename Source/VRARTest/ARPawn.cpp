// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPawn.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UIConnectionWidget.h"
#include "UIObserver.h"
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

	connectionWidget = CreateWidget<UUIConnectionWidget>(playerController, ConnectionWidgetClass);
	connectionWidget->setupUIObserver(this);
	connectionWidget->AddToViewport();	

}

// Called every frame
void AARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (showPlanes && connectionTypeSelected)
	{
		UpdatePlanes();
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

							if (hitActor->IsA(AMapSection::StaticClass())) {
								AMapSection* selectedMapSection = Cast<AMapSection>(hitActor);
								if (currentlySelectedMapSection == selectedMapSection) {
									currentlySelectedMapSection->swapSelectedMaterial();
									manager->spawnEntityAtSection(currentlySelectedMapSection, AARVRGameManager::ESpawnableObject::Chest);
									currentlySelectedMapSection->swapSelectedMaterial();
									currentlySelectedMapSection = nullptr;
									manager->switchTurns();
									GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Turn Changed")));
								}
								if (!currentlySelectedMapSection || currentlySelectedMapSection != selectedMapSection) {
									selectedMapSection->swapSelectedMaterial();
									currentlySelectedMapSection = selectedMapSection;
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
}

void AARPawn::SpawnMap()
{
	if (!planeSelected)
	{
		UE_LOG(LogTemp, Warning, TEXT("No plane selected."));
		return;
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

	float scaleFactor = FMath::Min(planeWidth / mapWidth, planeDepth / mapDepth);


	if (mapRoot)
	{
		FVector planeLocation = selectedPlane->GetActorLocation();
		FRotator planeRotation = selectedPlane->GetActorRotation();

		mapRoot->SetActorLocation(planeLocation);
		mapRoot->SetActorRotation(planeRotation);
		mapRoot->SetActorScale3D(FVector(scaleFactor));

		TArray<AActor*> mapRootChildren;
		mapRoot->GetAttachedActors(mapRootChildren);
		for (AActor* child : mapRootChildren)
		{
			if (child->Tags.Contains("ARMove"))
			{
				child->SetActorHiddenInGame(false);
			}

		}

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
	}
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
		connectionWidget->changeConfirmButtonVisibility();
		UStaticMeshComponent* planeMesh = planeActor->FindComponentByClass<UStaticMeshComponent>();
		planeMesh->SetMaterial(0, selectedPlaneMaterial);
		selectedPlane = planeActor;
		selectedPlaneGeometry = planeGeometry;

	}
	else if(selectedPlane == planeActor)
	{
		connectionWidget->changeConfirmButtonVisibility();
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