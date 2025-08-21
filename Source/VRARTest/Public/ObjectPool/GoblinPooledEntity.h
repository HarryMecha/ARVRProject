// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectPool/LivingPooledEntity.h"
#include "Components/BoxComponent.h"
class ALivingEntityAIController;
#include "GoblinPooledEntity.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API AGoblinPooledEntity : public ALivingPooledEntity
{
	GENERATED_BODY()

public:
	AGoblinPooledEntity();

	//Enables leftHandCollider for the ability to deal damage on hit
	void StartAttack() override;

	//Disables leftHandCollider
	void EndAttack() override;

	// changeState() is the functionality for the changes within the entities Finite State Machine the transition to different states will depend on which of the colliders 
	// the VR player is within.
	void changeState(ELivingEntityState newState) override;

	void toggleTransparent(bool toggle) override;

	void EnableAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//This function is called when the Goblin entity is possessed by the AIController it casts this controller to a ALivingEntityAIController to use it's functionalities.
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* LeftHandCollider;

	// This is the function called when the VR Player overlaps with the Goblin's left hand, this will only be enabled when the Goblin is in a specific frame of it's attacking animation.
	// A cooldown was also imposed so if the player has been hit within a certain window they weren't be hurt, if it's outside of that window it applies 1 damage to the player
	UFUNCTION()
	void OnLeftHandOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Plays Attack Animation
	void PlayAttackAnimation();

	//Plays Idle Animation
	void PlayIdleAnimation();

	//Plays Run Animation
	void PlayRunAnimation();

	// When the attack animation ends OnAttackMontageEnded will call this will change it's state to a different one depending on the collider the VR Player is in
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	bool playerBeenAttacked = false;

	ALivingEntityAIController* AIController;

};
