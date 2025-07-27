// Fill out your copyright notice in the Description page of Project Settings.


#include "TorchActorComponent.h"

// Sets default values for this component's properties
UTorchActorComponent::UTorchActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	torchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchMesh"));
	torchMesh->SetupAttachment(this);

	torchLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TorchLight"));
	torchLight->SetupAttachment(torchMesh);
}


// Called when the game starts
void UTorchActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


