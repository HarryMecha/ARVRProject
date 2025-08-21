// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/LivingEntityAIController.h"


void ALivingEntityAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ALivingEntityAIController::ChaseTarget(AActor* Target)
{
    if (Target)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Player is Target")));

        MoveToActor(Target, 5.0f);
    }
}

void ALivingEntityAIController::StopChasingTarget()
{
    StopMovement();
}