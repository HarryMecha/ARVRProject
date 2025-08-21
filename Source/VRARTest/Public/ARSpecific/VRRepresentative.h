// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "VRRepresentative.generated.h"

/*
	This class is simply for setting up the blueprint for the AR players, small Dwarf VR Representation, it's only referred to when syncing up the player positions in the PlayerMovementCommand
*/

UCLASS()
class VRARTEST_API AVRRepresentative : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRRepresentative();
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Components")
	UStaticMeshComponent* vrLeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Components")
	UStaticMesh* vrLeftHandOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Components")
	UStaticMesh* vrLeftHandClosed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Components")
	UStaticMesh* vrLeftHandHammer;

	UStaticMeshComponent* getVRLeftHand() { return vrLeftHand; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Components")
	UStaticMeshComponent* vrRightHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Components")
	UStaticMesh* vrRightHandOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Components")
	UStaticMesh* vrRightHandClosed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Components")
	UStaticMesh* vrRightHandHammer;

	UStaticMeshComponent* getVRRightHand() { return vrRightHand; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Components")
	UStaticMeshComponent* vrHead;

	UStaticMeshComponent* getVRHead() { return vrHead; }


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Components")
	USkeletalMeshComponent* vrBody;

	USkeletalMeshComponent* getVRBody() { return vrBody; }


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Components")
	UCapsuleComponent* capsuleCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* idleAnimation;

	UAnimationAsset* getIdleAnimation() { return idleAnimation; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* movingAnimation;

	UAnimationAsset* getMovingAnimation() { return movingAnimation; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	// Called every frame

};
