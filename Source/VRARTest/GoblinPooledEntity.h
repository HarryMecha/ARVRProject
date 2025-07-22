// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LivingPooledEntity.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
