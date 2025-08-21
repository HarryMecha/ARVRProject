// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectPool/StaticPooledEntity.h"
class UBoxComponent;
#include "TrapPooledEntity.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API ATrapPooledEntity : public AStaticPooledEntity
{
	GENERATED_BODY()
	
public:
	ATrapPooledEntity();

	void toggleMeshVisibility(bool toggle)
	{
		entityMesh->SetVisibility(toggle);
	}

	// resetTrap() renables the collision of the boxCollider and sets the visibility to true, also flipping the trap activated boolean to false.
	void resetTrap();

	void toggleTransparent(bool toggle) override;

protected:
	
	virtual void BeginPlay() override;

	// OnMeshBeginOverlap() funciton is called when the vr player overlaps with the boxCollider, if the trap has not already been activated it will add a random powerup all with detrimental 
	// effects onto the VR player and then call entityFinished() and return to the pool.
	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* entityMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* boxCollider;

private:
	bool trapActivated = false;

	bool isTransparent = false;
};
