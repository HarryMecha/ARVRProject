// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARBlueprintLibrary.h"
#include "ARSessionConfig.h"
#include "MapSection.h"
#include "GameFramework/Pawn.h"

class UUIObserver;
class UUIConnectionWidget;
class AARVRGameManager;

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

	void SpawnMap();

	void startARSession();

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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUIConnectionWidget> ConnectionWidgetClass;

	UUIConnectionWidget* connectionWidget;

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
};
