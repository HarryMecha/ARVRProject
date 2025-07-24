// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeartActor.generated.h"

UENUM(BlueprintType)
enum class EHeartFillState : uint8
{
	EMPTY UMETA(DisplayName = "Empty"),
	QUARTER UMETA(DisplayName = "Quarter"),
	HALF UMETA(DisplayName = "Half"),
	THREEQUARTER UMETA(DisplayName = "ThreeQuarter"),
	FULL UMETA(DisplayName = "Full")
};


UCLASS()
class VRARTEST_API AHeartActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeartActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* heartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* fullHeartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* threeQuarterHeartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* halfHeartMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* quarterHeartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* emptyHeartMesh;

	UFUNCTION(BlueprintCallable)
	void setHeartState(EHeartFillState newState);

	EHeartFillState getCurrentState()
	{
		return currentState;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EHeartFillState currentState;

};
