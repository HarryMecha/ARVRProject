// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ARPawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/TouchInterface.h"
#include "HeartActor.h"
#include "ARCombatUI.h"
#include "ResultsScreenWidget.h"
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void setManager(AARVRGameManager* arvrManager)
	{
		manager = arvrManager;
	}

	void setIsARSession(bool toggle)
	{
		isARSession = toggle;
	}

	float getMaxHealth()
	{
		return maxHealth;
	}

	float getCurrentMaxHealth()
	{
		return currentMaxHealth;
	}

	float getCurrentHealth()
	{
		return currentHealth;
	}

	void resetHealth() { currentHealth = currentMaxHealth; }

	void takeDamage(float amount);

	void Die();

	TArray<AHeartActor*> heartActors;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AHeartActor> heartActorClass;

	void CreateHealthUI();

	void DestroyHealthUI();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* heartRoot;

	UARCombatUI* getARCombatWidget() const;

	UResultsScreenWidget* getResultsScreenWidget() const;

	APlayerController* getPlayerController()
	{
		return playerController;
	}

	USceneComponent* getSpellSpawnPoint()
	{
		return spellSpawnPoint;
	}

	void spawnSpell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	void moveWizardForwardBack(float value);

	void moveWizardLeftRight(float value);

	void turnWizardLeftRight(float value);

	void turnWizardUpDown(float value);

	void hasCharacterMoved();

	float TimeSinceLastPacket = 0.0f;

	const float PacketInterval = 0.033f;

	AARVRGameManager* manager;

	bool isARSession = false;

	float maxHealth;

	float currentMaxHealth;

	float currentHealth;

};
