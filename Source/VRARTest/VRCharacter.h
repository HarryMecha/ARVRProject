// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetInteractionComponent.h"
class AARVRGameManager;
class UVRPlayerUI;
class UPowerUpSelectionMenu;
class UVRMapWidget;
class UMotionControllerComponent;
class UCameraComponent;
class USceneComponent;
class UBoxComponent;
class AMapTunnel;
class UResultsScreenWidget;
#include "VRCharacter.generated.h"

UENUM(BlueprintType)
enum class EObjectInHand : uint8
{
	Hammer UMETA(DisplayName = "Hammer"),
	Lantern UMETA(DisplayName = "Lantern"),
	Map UMETA(DisplayName = "Map"),
	Sleeping UMETA(DisplayName = "Sleeping")
};

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
	SPEED UMETA(DisplayName = "Speed"),
	SPEEDDOWN UMETA(DisplayName = "SpeedDown"),
	ATTACK UMETA(DisplayName = "Attack"),
	ATTACKDOWN UMETA(DisplayName = "AttackDown"),
	LANTERN UMETA(DisplayName = "Lantern"),
	HEALTH UMETA(DisplayName = "Health"),
	HEALTHDOWN  UMETA(DisplayName = "HealthDown"),
	NONE UMETA(DisplayName = "None")
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
	UCameraComponent* VRCamera;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* characterMesh;

	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* leftController;

	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* rightController;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* leftHandMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rightHandMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* leftHandCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* rightHandCollider;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* leftHandHammerMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rightHandHammerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* hammerPowerUpMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* hammerPowerDownMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* hammerRegularMaterial;

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	UStaticMesh* rightControllerLanternMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* backCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* leftControllerHammerCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* rightControllerHammerCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* vrUIComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVRPlayerUI* vrPlayerUI;

	UVRPlayerUI* getVRPlayerUI()
	{
		return vrPlayerUI;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Decleration")
	TSubclassOf<UVRPlayerUI> vrPlayerUIClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* powerUpMenuComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPowerUpSelectionMenu* powerUpSelectionMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Decleration")
	TSubclassOf<UPowerUpSelectionMenu> powerUpSelectionMenuClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* vrMapComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVRMapWidget* vrMapWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Decleration")
	TSubclassOf<UVRMapWidget> vrMapWidgetClass;

	UResultsScreenWidget* getResultsScreenWidget()
	{
		return resultsScreenWidget;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* resultsScreenWidgetComponent;

	UWidgetComponent* getResultsScreenWidgetComponent()
	{
		return resultsScreenWidgetComponent;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UResultsScreenWidget* resultsScreenWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class Decleration")
	TSubclassOf<UResultsScreenWidget> resultsScreenWidgetClass;

	AARVRGameManager* manager;

	float maxHealth;

	float getMaxHealth()
	{
		return maxHealth;
	}

	float currentHealth;

	void modifyHealth(float amount);

	void updateDamage(float health, bool isPlayer);

	void addPowerUp(EPowerUpType type);

	bool getHammerPowerUp()
	{
		return hammerPowerUp;
	}

	bool getHammerPowerDown()
	{
		return hammerPowerDown;
	}

	bool getSpeedPowerUp()
	{
		return speedPowerUp;
	}

	bool getSpeedPowerDown()
	{
		return speedPowerDown;
	}

	void turnOffHammerPowerUp();

	void turnOffHammerPowerDown();

	void turnOffSpeedPowerUp();

	bool speedPowerUpCheck();

	void turnOffSpeedPowerDown();

	bool speedPowerDownCheck();

	void turnOffLanternPowerUp();
	
	void setFinishedBool(bool toggle)
	{
		isFinished = toggle;
	}

protected:
	// Called when the game starts or when spawned 
	virtual void BeginPlay() override;

	void LeftTriggerPressed();
	void LeftTriggerReleased();
	
	void RightTriggerPressed();
	void RightTriggerReleased();

	void MenuOpenButtonPressed();

	void MapOpenButtonPressed();


	void HandleMenuAxisX(float value);
	void HandleMenuAxisY(float value);

	void getMenuSelectionIndex();

	void activatePowerUp();

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

	int controllerRayLength = 750;

	void hasCharacterMoved();

	float TimeSinceLastPacket = 0.0f;
	
	const float PacketInterval = 0.033f;

	bool enemyHit = false;
	
	FTimerHandle enemyHitResetTimerHandle;

	int numberOfHammerPU = 1;

	int numberOfSpeedPU = 1;

	int numberOfPotionPU = 1;

	int numberOfLanternPU = 1;

	int currentSelectedIndex = -1;

	bool powerUpMenuOpen = false;

	bool mapOpen = false;

	bool hammerPowerUp = false;

	bool speedPowerUp = false;

	bool hammerPowerDown = false;

	bool speedPowerDown = false;

	bool lanternPowerUp = false;

	bool hammerEquipped = false;

	int speedPowerUpCounter = 0;

	int speedPowerDownCounter = 0;

	FVector2D StickInput;

	AMapTunnel* selectedTunnel = nullptr;

	bool isFinished = false;
};
