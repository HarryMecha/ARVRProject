// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "VRCharacter.h"
#include "PowerUpActor.generated.h"

UCLASS()
class VRARTEST_API APowerUpActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerUpActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimationAsset* hoverAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power-Up")
	EPowerUpType powerUpType;

	void setVisible()
	{
		mesh->SetVisibility(true);
		mesh->PlayAnimation(hoverAnimation, true);
		TArray<USceneComponent*> meshChildren;
		mesh->GetChildrenComponents(true, meshChildren);
		if (meshChildren.Num() > 0)
		{
			for (USceneComponent* child : meshChildren)
			{
				child->SetVisibility(true);
			}
		}
	}

	void adjustCollision()
	{
		boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};