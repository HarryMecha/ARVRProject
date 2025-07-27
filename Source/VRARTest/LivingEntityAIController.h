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

	void ChaseTarget(AActor* Target);

	void StopChasingTarget();
};
