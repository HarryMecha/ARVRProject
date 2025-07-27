// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "TorchActor.generated.h"

UCLASS()
class VRARTEST_API ATorchActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorchActor();

	void toggleLight();

	virtual void Tick(float DeltaTime) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* torchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UPointLightComponent* torchLight;

private:
	bool fadeIn = false;

	float initialIntensity;

	float currentIntensity;

	float fadeTimer = 0.0f;

	float maxFadeTime = 2.5f;

};
