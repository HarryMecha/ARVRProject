// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartActor.h"

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
	SetHeartState(EHeartFillState::FULL); // Default state

}

void AHeartActor::SetHeartState(EHeartFillState NewState)
{
    switch (NewState)
    {
    case EHeartFillState::FULL:
        heartMesh->SetStaticMesh(fullHeartMesh);
        break;
    case EHeartFillState::THREEQUARTER:
        heartMesh->SetStaticMesh(threeQuarterHeartMesh);
        break;
    case EHeartFillState::HALF:
        heartMesh->SetStaticMesh(halfHeartMesh);
        break;
    case EHeartFillState::QUARTER:
        heartMesh->SetStaticMesh(quarterHeartMesh);
        break;
    case EHeartFillState::EMPTY:
        heartMesh->SetStaticMesh(emptyHeartMesh);
        break;
    }
}

