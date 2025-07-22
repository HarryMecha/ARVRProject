// Fill out your copyright notice in the Description page of Project Settings.


#include "LivingPooledEntity.h"

ALivingPooledEntity::ALivingPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // Create and attach mesh
    mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    mesh->SetupAttachment(RootComponent);
}

void ALivingPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    mesh->PlayAnimation(idleAnimation, true);
}