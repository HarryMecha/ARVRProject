// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LivingPooledEntity.h"
#include "Components/BoxComponent.h"
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

	void StartAttack() override;

	void EndAttack() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* LeftHandCollider;

	UFUNCTION()
	void OnLeftHandOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
