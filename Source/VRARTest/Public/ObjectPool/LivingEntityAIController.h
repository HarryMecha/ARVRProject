// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LivingEntityAIController.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API ALivingEntityAIController : public AAIController
{
	GENERATED_BODY()
	

public:
	void BeginPlay() override;

	//ChaseTarget will call the AIControllers MoveToActor function which will cause the entity to move toward the passed in Actor
	// if they are within the Navigational Mesh
	void ChaseTarget(AActor* Target);

	void StopChasingTarget();
};
