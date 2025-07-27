// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PointLightComponent.h"
#include "TorchActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRARTEST_API UTorchActorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTorchActorComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* torchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UPointLightComponent* torchLight;

};
