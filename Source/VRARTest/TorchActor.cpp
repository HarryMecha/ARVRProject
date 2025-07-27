// Fill out your copyright notice in the Description page of Project Settings.


#include "TorchActor.h"

// Sets default values
ATorchActor::ATorchActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	torchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TorchMesh"));
	RootComponent = torchMesh;

	torchLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("TorchLight"));
	torchLight->SetupAttachment(torchMesh);
	torchLight->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

}

// Called when the game starts or when spawned
void ATorchActor::BeginPlay()
{
	Super::BeginPlay();
	
    initialIntensity = torchLight->Intensity;

    currentIntensity = 0.0f;

    torchLight->SetVisibility(false);

}

void ATorchActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (fadeIn)
    {
        fadeTimer += DeltaTime;

        float alpha = fadeTimer / maxFadeTime;
        currentIntensity = FMath::Lerp(0.0f, initialIntensity, alpha);

        torchLight->SetIntensity(currentIntensity);

        if (alpha >= 1.0f)
        {
            fadeIn = false;
        }
    }

}

void ATorchActor::toggleLight()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,FString::Printf(TEXT("Toggling Light")));
    torchLight->SetVisibility(true);
    fadeIn = true;
}

