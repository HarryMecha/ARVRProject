// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticPooledEntity.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
};
