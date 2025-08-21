// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARPawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/TouchInterface.h"
#include "UI/HeartActor.h"
#include "UI/ARCombatUI.h"
#include "UI/ResultsScreenWidget.h"
#include "ARCharacter.generated.h"

UCLASS()
class VRARTEST_API AARCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AARCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// The setter method for the pointer to an instance of the ARVRManager class.
	void setManager(AARVRGameManager* arvrManager)
	{
		manager = arvrManager;
	}

	// The setter method for the boolean isARSession.

	void setIsARSession(bool toggle)
	{
		isARSession = toggle;
	}

	// Getter Method for Max Health.
	float getMaxHealth()
	{
		return maxHealth;
	}

	// Getter Method for the current max health of the AR Character.
	float getCurrentMaxHealth()
	{
		return currentMaxHealth;
	}

	// Getter Method for the current health of the AR Character.
	float getCurrentHealth()
	{
		return currentHealth;
	}

	// Resets the AR Characters Health back to it's maxHealth.
	void resetHealth() { currentHealth = currentMaxHealth; }

	// Take Damage reduces the current health by the amount given in the parameter. This is then visually changed aswell by changing the 
	// model of the heartActors that make up the health bar to reflect this new amount.
	void takeDamage(float amount);

	//The function that runs when the ARCharacter's health is reduced to 0, this will destroy the HealthBar.
	void Die();

	TArray<AHeartActor*> heartActors; //An array of Heart Actors, the health bar above the AR Character.

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AHeartActor> heartActorClass;

	// CreateHealthUI spawns heartActor objects above the ARCharacter, the amount of hearts depending on their currentMaxHealth, these actors are adde to the heartActors array.
	void CreateHealthUI();

	// This will clear the heartActors array and destory all the heart actor objects.
	void DestroyHealthUI();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* heartRoot;

	// Getter function for ARCombatWidget pointer.
	UARCombatUI* getARCombatWidget() const;

	// Getter function for Results Screen pointer.
	UResultsScreenWidget* getResultsScreenWidget() const;

	// Getter function for playerController pointer.
	APlayerController* getPlayerController()
	{
		return playerController;
	}

	// Getter function for spell Spawn Point pointer.
	USceneComponent* getSpellSpawnPoint()
	{
		return spellSpawnPoint;
	}

	// The spawnSpell function will call getEntityFromPool with the type spelland create a command to be send over the 
	// network to inform the VR Player to do the same.
	void spawnSpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Inherited PossedBY function, is called when the AR player takes over this character, the activates the touch interface for movement control.
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* camera;

	UPROPERTY(EditDefaultsOnly, Category = "Touch Interface")
	 UTouchInterface* currentTouchInterface;

	 UPROPERTY(EditAnywhere, Category = "UI")
	 TSubclassOf<UARCombatUI> arCombatWidgetBlueprintClass;

	 UPROPERTY()
	 UARCombatUI* arCombatWidget;

	 UPROPERTY(EditAnywhere, Category = "UI")
	 TSubclassOf<UResultsScreenWidget> resultsWidgetBlueprintClass;

	 UPROPERTY()
	 UResultsScreenWidget* resultstWidget;

	 UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	 USceneComponent* spellSpawnPoint;
private:	

	APlayerController* playerController;

	FVector prevCharacterPosition;

	FRotator prevCharacterRotation;

	// The moveWizardForwardBack function takes a value given by the users input on the left touch control it will then add a MovementInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to move forward and back.
	void moveWizardForwardBack(float value);

	// The moveWizardLeftRight function takes a value given by the users input on the left touch control it will then add a MovementInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to move left and right.
	void moveWizardLeftRight(float value);

	// The turnWizardLeftRight function takes a value given by the users input on the right touch control it will then add a AddControllerYawInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to turn left and right.
	void turnWizardLeftRight(float value);

	// The turnWizardUpDown function takes a value given by the users input on the right touch control it will then add a AddControllerPitchInput to the character
	// based on the strength of that input and the currently facing direction of the character. Allowing the Character to look up and down.
	void turnWizardUpDown(float value);

	// The hasCharacterMoved function checks the current position and rotation of the ARCharacter and compares this to the position and rotation set when the previous
	// sent WizardMovement command was sent, if it finds a change it will then send a WizardMovement command to the other player in order to update their position within
	// their game.
	void hasCharacterMoved();

	float TimeSinceLastPacket = 0.0f; // Time since last WizardMovement() command has been sent.

	const float PacketInterval = 0.033f; // Interval at which positions are checked and commands are sent.

	AARVRGameManager* manager;

	bool isARSession = false;

	float maxHealth;

	float currentMaxHealth;

	float currentHealth;

};
