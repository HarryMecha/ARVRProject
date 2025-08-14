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

	float currentMaxHealth;

	float getMaxHealth()
	{
		return maxHealth;
	}

	float getCurrentMaxHealth()
	{
		return currentMaxHealth;
	}

	float currentHealth;

	float getCurrentHealth()
	{
		return currentHealth;
	}

	void resetHealth() { currentHealth = currentMaxHealth; }

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

	void DestroyHealthUI();

	virtual void StartAttack() PURE_VIRTUAL(ALivingPooledEntity::StartAttack, );

	virtual void EndAttack() PURE_VIRTUAL(ALivingPooledEntity::EndAttack, );

	virtual void changeState(ELivingEntityState newState) PURE_VIRTUAL(ALivingPooledEntity::changeState, );

	FVector getEntityPosition() { return RootComponent->GetRelativeLocation(); };

	FRotator getEntityRotation() { return RootComponent->GetRelativeRotation(); };

	void setEntityPosition(FVector newPosition) { RootComponent->SetRelativeLocation(newPosition); };

	void setEntityRotation(FRotator newRotation) { RootComponent->SetRelativeRotation(newRotation); };

	void toggleFrenzyVR(bool toggle);

	void toggleFrenzyAR(bool toggle);


	bool getIsFrenzied()
	{
		return isFrenzied;
	}

	virtual void toggleTransparent(bool toggle) PURE_VIRTUAL(AStaticPooledEntity::toggleTransparent, );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPooledEntityComponent* poolComponent;


	float verticalHeartOffset = 0.0f;


	float currentVerticalHeartOffset = 0.0f;

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

	bool isFrenzied = false;

	FVector originalScale;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* regularMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* frenzyMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials")
	UMaterialInterface* transparentMaterial;


};
