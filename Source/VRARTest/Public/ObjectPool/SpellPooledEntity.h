// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectPool/StaticPooledEntity.h"
#include "ARVRGameManager.h"
class UBoxComponent;
class UProjectileMovementComponent;
#include "SpellPooledEntity.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API ASpellPooledEntity : public AStaticPooledEntity
{
	GENERATED_BODY()
	
public:

	ASpellPooledEntity();

	// startTimeToLive() starts a checkers if the timerHandle is currently active if it is it will clear it, it will then set up the timer, using timerHandle as the timer to start,
	// this to reference the entity the timer is applied to, the onTimeToLiveExpired() function as the one to perform once the timer has finished, with the duration being set at timeToLive
	// and loop set to false.
	void startTimeToLive();

	void setManager(AARVRGameManager* arvrmanager)
	{
		manager = arvrmanager;
	}

	UProjectileMovementComponent* getProjectileMovement()
	{
		return projectileMovement;
	}

	// onTimeToLiveExpired() calls the function nonPlacableObjectConclusion() within ARVRManager to attempt to return it to the pool for reuse.
	UFUNCTION()
	void onTimeToLiveExpired();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* entityMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell Properties")
	float timeToLive;

	UPROPERTY()
	UProjectileMovementComponent* projectileMovement;

	// OnColliderOverlap() handles the collsion of the entity with the VR Player if it succesfully hits the players capsule collider it will inflict 1 point of damage on them,
	// and onTimeToLiveExpired() is prematurely called returning it to the pool. The same logic is applied to the vrRepresntative however leaving out the modify health call.
	UFUNCTION()
	void OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:

	FTimerHandle timerHandle;


	AARVRGameManager* manager;
};
