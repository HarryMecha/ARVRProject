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

	// Called to bind method functionality to input on the PLayerInputComponent
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// The MoveForwardBack function takes a speed value given by the users input on the left motion controller it will then add a MovementInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to move forward and back.
	void MoveForwardBack(float Speed);

	// The MoveLeftRight function takes a speed value given by the users input on the right motion controller it will then add a MovementInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to move left and right.
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

	// modifyHealth is used when the VR Player either loses or gains health this will make ammendments to all UI elements and check to see if the lose condition
	// has been achieved by setting the VR Players health to zero.
	void modifyHealth(float amount);

	// updateDamage() is a helper function used to create and send UpdateHealthCommands, using the bool to determine whether it is the VR Player or the enemy who has taken damage.
	void updateDamage(float health, bool isPlayer);

	//addPowerUp() will use the received EPowerUpType in a switchcase. Most of these cases simply add an additional int to the count for their respective powerup, however for
	// powerDowns recieved by traps, the logic for disadvantaging the player is handled here.
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

	//turnOffHammerPowerUp() handles the loss of the effects of the HammerPowerUp resetting the boolean and the UI elements aswell as any visual effects that had been applied
	void turnOffHammerPowerUp();

	//turnOffHammerPowerDown() handles the loss of the effects of the HammerPowerDown resetting the boolean and the UI elements aswell as any visual effects that had been applied
	void turnOffHammerPowerDown();

	//turnOffSpeedPowerUp() handles the loss of the effects of the SpeedPowerUp, resetting the boolean and the UI elements.
	void turnOffSpeedPowerUp();

	//speedPowerUpCheck functions performs a check to see if the turn count since the powerup was applied has reached 2 in order to turn off functionality. When it is called it will incriment
	// a timer value.
	bool speedPowerUpCheck();

	//turnOffSpeedPowerDown() handles the loss of the effects of the SpeedPowerDown, resetting the boolean and the UI elements.
	void turnOffSpeedPowerDown();

	//speedPowerDownCheck functions performs a check to see if the turn count since the powerup was applied has reached 2 in order to turn off functionality. When it is called it will incriment
	// a timer value.
	bool speedPowerDownCheck();

	//turnOffLanternPowerUp() handles the loss of the effects of the LanternPowerUp turning off the fog on the selected map tunnel and setting all visual elements  and variables 
	// associated with the powerup to their original states.
	void turnOffLanternPowerUp();
	
	void setFinishedBool(bool toggle)
	{
		isFinished = toggle;
	}

protected:
	// Called when the game starts or when spawned 
	virtual void BeginPlay() override;

	//LeftTriggerPressed() handles functionality for when the trigger on the left motion controller is pressed, this includes logic including changing the mesh of the hand from open to closed,
	// or from open to holding a hammer if the hand is within the back collider. It will also simulate a mouse click if pointed at widget elements in 3D space. 
	void LeftTriggerPressed();
	
	//LeftTriggerReleased() will reset any changed made by LeftTriggerPressed().
	void LeftTriggerReleased();
	
	//RightTriggerPressed() handles functionality for when the trigger on the right motion controller is pressed, this includes logic including changing the mesh of the hand from open to closed,
	// or from open to holding a hammer if the hand is within the back collider. It will also simulate a mouse click if pointed at widget elements in 3D space. If the lantern power up is equipped this trigger
	// can be used to select the mapTunnel you want to remove fog from.
	void RightTriggerPressed();

	//RightTriggerReleased() will reset any changed made by RightTriggerPressed().
	void RightTriggerReleased();

	//MenuOpenButtonPressed() will make the powerup menu ui widget attached to the left hand visible or invisible depending on if the map was already open, it also calls for a
	// reloadMenu incase any of the values of powerups have changed since it was last open.
	void MenuOpenButtonPressed();

	//MapOpenButtonPressed() will make the map ui widget attached to the right hand visible or invisible depending on if the map was already open.
	void MapOpenButtonPressed();

	// When the powerupMenu is open HandleMenuAxisX will set the value of StickInputs X component to the current vertical value of the the left stick.
	void HandleMenuAxisX(float value);

	// When the powerupMenu is open HandleMenuAxisY will set the value of StickInputs Y component to the current horizontal value of the the left stick.
	void HandleMenuAxisY(float value);

	// getMenuSelectionIndex() converts the value of the StickInputs in an angle, this is done by first apply atan^2 and then converting that value from degrees to radians, from there
	// the value is clamped to be with 360 degrees, a circle like the menu, and the angle size of each is set to 72, 360/5 one for each powerup menu section, the value is then divided by the
	// section size and modulo 5 is applied in order to get an index number that can be used to select a slice on the powerupMenu as it will correspond to the index number of the map.
	void getMenuSelectionIndex();

	//activatePowerUp() uses a switch case based on the currentSelectedIndex of the powerUpMenu, this will then remove 1 from the count of whatever powerup was used, activate the boolean
	// associated with the powerup to provide access to functionality elsewhere and applied changes to any visual effects these power ups apply.
	void activatePowerUp();

	// controllerLineTrace() function first gets the currently selected and shoots a ray in the forward direction of that controller, this ray will continue until it reaches it's size limit or it
	// hits an object. If the Player has the lanternPowerUp enabled they will be able to use this to select an unvisited tunnel forwhich they want to remove the fog from. This can also be used to point
	// at widget elements in 3D space with the right or left trigger simulating mouse clicks on whatever button the player is pointing at. This ray is visualised by distorting the proportions of a
	// cylindrical mesh to match the direction and the start and end point of the ray cast.
	void controllerLineTrace(UMotionControllerComponent* controller);

	// The TurnLeftRight function takes a value given by the users input on the right  motion controller it will then add a AddControllerYawInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to turn left and right.
	void TurnLeftRight(float value);

	//OverlapBegin in the VRCharacter mainly handles two scenarios whether either of the hands is overlapping with the backcollider allowing for the hammer to be equipped of if the collider
	// attached to the hammer has hit an enemy, in which is applies damage to them or projectile, in which case it returns that entity back to the pool.
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//OverlapEnd() similarly handles two scenarios the hands exiting the backcolldier and losing the ability to equip the hammer or the hammer collider leaving a collided enemy, in which case a
	// cooldown timer is started in order to now allow the spamming of attack due to rapid movement of the motion controllers.
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

	// The hasCharacterMoved function checks the current position and rotation of all the different components of the VRCharacters and compares this to the position and rotation set when the previous
	// sent PlayerMovement command was sent, if it finds a change it will then send a PlayerMovement command to the other player in order to update their position within
	// their game.
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
