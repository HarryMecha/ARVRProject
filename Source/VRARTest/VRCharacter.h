// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetInteractionComponent.h"
class AARVRGameManager;
#include "VRCharacter.generated.h"


UCLASS()
class VRARTEST_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:	

	AVRCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void MoveForwardBack(float Speed);

	void MoveLeftRight(float Speed);

	// Components
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* characterMesh;

	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* leftController;

	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* rightController;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* leftHandMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rightHandMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* leftControllerRayMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rightControllerRayMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* leftControllerOpenMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* leftControllerClosedMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* rightControllerOpenMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* rightControllerClosedMesh;

	AARVRGameManager* manager;

protected:
	// Called when the game starts or when spawned 
	virtual void BeginPlay() override;

	void LeftTriggerPressed();
	void LeftTriggerReleased();
	
	void RightTriggerPressed();
	void RightTriggerReleased();

	void controllerLineTrace(UMotionControllerComponent* controller);

	void TurnLeftRight(float value);

private:
	APlayerController* playerController;

	UWidgetInteractionComponent* leftWidgetInteraction;

	UWidgetInteractionComponent* rightWidgetInteraction;

	bool hasMoved;

	FVector prevCharacterPosition;

	FVector prevCameraPosition;

	FVector prevLeftHandPosition;

	FVector prevRightHandPosition;

	FRotator prevCharacterRotation;

	FRotator prevCameraRotation;

	FRotator prevLeftHandRotation;

	FRotator prevRightHandRotation;

	bool leftControllerSelected = false;

	bool rightControllerSelected = false;

	int controllerRayLength = 500;

	void hasCharacterMoved();

	float TimeSinceLastPacket = 0.0f;
	
	const float PacketInterval = 0.033f;

};
