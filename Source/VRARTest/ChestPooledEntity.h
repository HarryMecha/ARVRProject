// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticPooledEntity.h"
#include "Components/BoxComponent.h"
#include "ChestPooledEntity.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API AChestPooledEntity : public AStaticPooledEntity
{
	GENERATED_BODY()
	
public:
	AChestPooledEntity();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* openAnimation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* entitySkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* boxCollider;

	FTimerHandle OpacityTimer;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dynamic Material")
	UMaterialInstanceDynamic* dynamicMaterial;

private:

	bool animationPlayed = false;

	bool fadeOut = false;

	float currentOpacity = 1.0f;

	float fadeTimer = 0.0f;

	float maxFadeTime = 1.5f; //MAYBE CHANGE IT MIGHT BE A BIT SLOW


};
