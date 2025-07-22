// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledEntity.h"
#include "PooledEntityInterface.h"
#include "PooledEntityComponent.h"
#include "GameFramework/Character.h"
#include "LivingPooledEntity.generated.h"

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

	float maxHealth;

	float currentHealth;

	void resetHealth() { currentHealth = maxHealth; }

	void takeDamage(float amount);

    void Die();

	IPooledEntityInterface* getPoolInterface() const 
	{ 
		return Cast<IPooledEntityInterface>(poolComponent); 
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPooledEntityComponent* poolComponent;
};
