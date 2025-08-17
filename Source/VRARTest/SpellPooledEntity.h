// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticPooledEntity.h"
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

	void startTimeToLive();

	void setManager(AARVRGameManager* arvrmanager)
	{
		manager = arvrmanager;
	}

	UProjectileMovementComponent* getProjectileMovement()
	{
		return projectileMovement;
	}

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

	UFUNCTION()
	void OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:

	FTimerHandle timerHandle;

	UFUNCTION()
	void onTimeToLiveExpired();

	AARVRGameManager* manager;
};
