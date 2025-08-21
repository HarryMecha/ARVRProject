// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HeartActor.h"

// Sets default values
AHeartActor::AHeartActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	heartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeartMesh"));
	RootComponent = heartMesh;
}

// Called when the game starts or when spawned
void AHeartActor::BeginPlay()
{
	Super::BeginPlay();
	setHeartState(EHeartFillState::FULL); 
}

void AHeartActor::setHeartState(EHeartFillState newState)
{
    switch (newState)
    {
    case EHeartFillState::FULL:
        heartMesh->SetStaticMesh(fullHeartMesh);
        currentState = EHeartFillState::FULL;
        break;
    case EHeartFillState::THREEQUARTER:
        heartMesh->SetStaticMesh(threeQuarterHeartMesh);
        currentState = EHeartFillState::THREEQUARTER;
        break;
    case EHeartFillState::HALF:
        heartMesh->SetStaticMesh(halfHeartMesh);
        currentState = EHeartFillState::HALF;
        break;
    case EHeartFillState::QUARTER:
        heartMesh->SetStaticMesh(quarterHeartMesh);
        currentState = EHeartFillState::QUARTER;
        break;
    case EHeartFillState::EMPTY:
        heartMesh->SetStaticMesh(emptyHeartMesh);
        currentState = EHeartFillState::EMPTY;
        break;
    }
}

