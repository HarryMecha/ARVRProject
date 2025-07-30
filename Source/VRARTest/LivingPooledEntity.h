// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PooledEntity.h"
#include "PooledEntityInterface.h"
#include "PooledEntityComponent.h"
#include "GameFramework/Character.h"
#include "HeartActor.h"
#include "Components/SphereComponent.h"
#include "LivingPooledEntity.generated.h"

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

	float getMaxHealth()
	{
		return maxHealth;
	}

	float currentHealth;

	float getCurrentHealth()
	{
		return currentHealth;
	}

	void resetHealth() { currentHealth = maxHealth; }

	void takeDamage(float amount);

    void Die();

	IPooledEntityInterface* getPoolInterface() const 
	{ 
		return Cast<IPooledEntityInterface>(poolComponent); 
	}

	TArray<AHeartActor*> heartActors;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<AHeartActor> heartActorClass;

	void CreateHealthUI();

	virtual void StartAttack() PURE_VIRTUAL(ALivingPooledEntity::StartAttack, );

	virtual void EndAttack() PURE_VIRTUAL(ALivingPooledEntity::EndAttack, );

	virtual void changeState(ELivingEntityState newState) PURE_VIRTUAL(ALivingPooledEntity::changeState, );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPooledEntityComponent* poolComponent;


	float verticalHeartOffset = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* heartRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* attackRangeCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* chaseRangeCollider;

	UFUNCTION()
	void OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackRangeExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnChaseRangeExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	ELivingEntityState currentState;

	bool playerInChaseCollider = false;

	bool playerInAttackCollider = false;

	bool isChasing = false;

	bool isAttacking = false;

	AActor* playerRef = nullptr;

	bool isTransparent = false;

};
