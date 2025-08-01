// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticPooledEntity.h"
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

	void resetTrap();

	void toggleTransparent(bool toggle) override;

protected:
	
	virtual void BeginPlay() override;

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
