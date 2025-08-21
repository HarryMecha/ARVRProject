// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledEntity.h"
#include "PooledEntityInterface.h"
#include "PooledEntityComponent.h"
#include "GameFramework/Character.h"
#include "UI/HeartActor.h"
#include "Components/SphereComponent.h"
#include "ARVRGameManager.h"
#include "LivingPooledEntity.generated.h"

//This enum defines the 3 states that are used in the Living Entities Finite State Machine
UENUM(BlueprintType)
enum class ELivingEntityState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Chasing  UMETA(DisplayName = "Chasing"),
	Attacking  UMETA(DisplayName = "Attacking")
};

/**
 * 
 */
UCLASS()
class VRARTEST_API ALivingPooledEntity : public ACharacter
{
	GENERATED_BODY()

public:
	ALivingPooledEntity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* idleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* runAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* attackMontage;

	float maxHealth;

	float currentMaxHealth;

	float getMaxHealth()
	{
		return maxHealth;
	}

	float getCurrentMaxHealth()
	{
		return currentMaxHealth;
	}

	float currentHealth;

	float getCurrentHealth()
	{
		return currentHealth;
	}

	void resetHealth() { currentHealth = currentMaxHealth; }

	// Take Damage reduces the current health by the amount given in the parameter. This is then visually changed aswell by changing the 
	// model of the heartActors that make up the health bar to reflect this new amount.
	void takeDamage(float amount);

	//The function that runs when the entity's health is reduced to 0, this will destroy the HealthBar and call the interactionConclusion() function in it's owning mapsection
	// to allow it to return to the pool
    void Die();

	IPooledEntityInterface* getPoolInterface() const 
	{ 
		return Cast<IPooledEntityInterface>(poolComponent); 
	}

	TArray<AHeartActor*> heartActors;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AHeartActor> heartActorClass;

	// CreateHealthUI spawns heartActor objects above the Entity, the amount of hearts depending on their currentMaxHealth, these actors are adde to the heartActors array.
	void CreateHealthUI();

	// This will clear the heartActors array and destory all the heart actor objects.
	void DestroyHealthUI();

	//Virtual function to be overwritten with new specific functionality by the subclasses of this class e.g. GoblinPooledEntity
	virtual void StartAttack() PURE_VIRTUAL(ALivingPooledEntity::StartAttack, );

	//Virtual function to be overwritten with new specific functionality by the subclasses of this class e.g. GoblinPooledEntity
	virtual void EndAttack() PURE_VIRTUAL(ALivingPooledEntity::EndAttack, );

	//Virtual function to be overwritten with new specific functionality by the subclasses of this class regarding the finite state machine e.g. GoblinPooledEntity
	virtual void changeState(ELivingEntityState newState) PURE_VIRTUAL(ALivingPooledEntity::changeState, );

	FVector getEntityPosition() { return RootComponent->GetRelativeLocation(); };

	FRotator getEntityRotation() { return RootComponent->GetRelativeRotation(); };

	void setEntityPosition(FVector newPosition) { RootComponent->SetRelativeLocation(newPosition); };

	void setEntityRotation(FRotator newRotation) { RootComponent->SetRelativeRotation(newRotation); };

	// Toggle function for frenzy specifically for VR players as the scaling needed to be different
	void toggleFrenzyVR(bool toggle);

	// Toggle function for frenzy specifically for AR players as the scaling needed to be different
	void toggleFrenzyAR(bool toggle);


	bool getIsFrenzied()
	{
		return isFrenzied;
	}

	virtual void toggleTransparent(bool toggle) PURE_VIRTUAL(AStaticPooledEntity::toggleTransparent, );

	void setPlayerRole(EPlayerRole role)
	{
		playerRole = role;
	}


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPooledEntityComponent* poolComponent;


	float verticalHeartOffset = 0.0f;


	float currentVerticalHeartOffset = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* heartRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* attackRangeCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* chaseRangeCollider;

	// Will change the current state of the entity to attacking when the VR actor enters the attackRangeCollider.
	UFUNCTION()
	void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Will change the current state of the entity to chasing when the VR actor enters the chaseRangeCollider.
	UFUNCTION()
	void OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Will change the current state of the entity to chasing when the VR actor enters the chaseRangeCollider.
	UFUNCTION()
	void OnAttackRangeExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Will change the current state of the entity to idle when the VR actor exits the chaseRangeCollider.
	UFUNCTION()
	void OnChaseRangeExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	ELivingEntityState currentState;

	bool playerInChaseCollider = false;

	bool playerInAttackCollider = false;

	bool isChasing = false;

	bool isAttacking = false;

	AActor* playerRef = nullptr;

	bool isTransparent = false;

	bool isFrenzied = false;

	FVector originalScale;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* regularMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* frenzyMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* transparentMaterial;

	EPlayerRole playerRole;
};
