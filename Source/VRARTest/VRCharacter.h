// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetInteractionComponent.h"
class AARVRGameManager;
class UVRPlayerUI;
#include "VRCharacter.generated.h"

UENUM(BlueprintType)
enum class EObjectInHand : uint8
{
	Hammer UMETA(DisplayName = "Hammer"),
	Lantern UMETA(DisplayName = "Lantern"),
	Map UMETA(DisplayName = "Map"),
	Sleeping UMETA(DisplayName = "Sleeping")
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* leftHandCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* rightHandCollider;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* leftHandHammerMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rightHandHammerMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* leftControllerRayMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rightControllerRayMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* leftControllerOpenMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* leftControllerClosedMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* leftControllerHammerMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* rightControllerOpenMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* rightControllerClosedMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* rightControllerHammerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* backCollider;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* lanternCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* sleepingBagCollider;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* leftControllerHammerCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* rightControllerHammerCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* vrUIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UVRPlayerUI* vrPlayerUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Decleration")
	TSubclassOf<UVRPlayerUI> vrPlayerUIClass;

	AARVRGameManager* manager;

	float maxHealth;

	float getMaxHealth()
	{
		return maxHealth;
	}

	float currentHealth;

	void takeDamage(float amount);

	void updateDamage(float health, bool isPlayer);

protected:
	// Called when the game starts or when spawned 
	virtual void BeginPlay() override;

	void LeftTriggerPressed();
	void LeftTriggerReleased();
	
	void RightTriggerPressed();
	void RightTriggerReleased();

	void controllerLineTrace(UMotionControllerComponent* controller);

	void TurnLeftRight(float value);

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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

	bool leftControllerInBackCollider = false;

	bool rightControllerInBackCollider = false;

	int controllerRayLength = 500;

	void hasCharacterMoved();

	float TimeSinceLastPacket = 0.0f;
	
	const float PacketInterval = 0.033f;

	bool enemyHit = false;

};
