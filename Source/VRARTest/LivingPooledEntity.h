// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledEntity.h"
#include "LivingPooledEntity.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API ALivingPooledEntity : public APooledEntity
{
	GENERATED_BODY()

public:
	ALivingPooledEntity();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* idleAnimation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
