// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinPooledEntity.h"

AGoblinPooledEntity::AGoblinPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;
    maxHealth = 5;
    resetHealth();
    verticalHeartOffset = 45.0f;
}


void AGoblinPooledEntity::BeginPlay()
{
    Super::BeginPlay();
}