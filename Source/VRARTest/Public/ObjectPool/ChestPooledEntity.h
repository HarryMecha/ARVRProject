// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectPool/StaticPooledEntity.h"
#include "Components/BoxComponent.h"
class APowerUpActor;
#include "ChestPooledEntity.generated.h"

UCLASS()
class VRARTEST_API AChestPooledEntity : public AStaticPooledEntity
{
	GENERATED_BODY()
	
public:
	AChestPooledEntity();

	//The Tick funtion in the ChestPooledEntity is used to control the transition of the material from opaque to transparent, using the current DeltaTime to determine the opacity.
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* openAnimation;

	void setPowerUpPickedUp()
	{
		powerUpPickedUp = true;
	}

	// ResetChest() function reset the material instance turning the object back to opaque, resets the timer and all the boolean values as well as re-enabling it's collider this is
	// called when it returns back to the pool so it can be reused
	void resetChest();

	void toggleTransparent(bool toggle) override;

protected:
	virtual void BeginPlay() override;

	// The OnMeshBeginOverlap() function activates when the chest is interacted with by the VR Players hand mesh, this will select a random powerup from the array of powerUpActors
	// setting it to visible and starting the fading out process for the mesh.
	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* entitySkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* boxCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* powerUpHolder;

	FTimerHandle OpacityTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dynamic Material")
	UMaterialInstanceDynamic* dynamicMaterial;

	TArray<APowerUpActor*> powerUpActors;

	APowerUpActor* selectedPowerUp;

private:

	bool animationPlayed = false;

	bool fadeOut = false;

	float currentOpacity = 1.0f;

	float fadeTimer = 0.0f;

	float maxFadeTime = 2.5f;

	bool chestOpened = false;

	bool powerUpPickedUp = false;
};
