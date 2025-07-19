// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolActor.h"
#include <Kismet/GameplayStatics.h>


// Sets default values
AObjectPoolActor::AObjectPoolActor()
{

}

// Called when the game starts or when spawned
void AObjectPoolActor::BeginPlay()
{
	Super::BeginPlay();

}

void AObjectPoolActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AObjectPoolActor::initialisePool(TSubclassOf<APooledEntity> poolTypeClass) 
{
    PooledEntityClass = poolTypeClass;
    TArray<AActor*> AllActors;
    AActor* mapRoot = nullptr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    for (AActor* actor : AllActors)
    {
        if (actor->Tags.Contains("MapRoot"))
        {
            mapRoot = actor;
        }
    }


    EntityPool.SetNum(poolSize);

    for (int i = 0; i < poolSize; i++) {
        EntityPool[i] = GetWorld()->SpawnActor<APooledEntity>(PooledEntityClass);
        EntityPool[i]->AttachToActor(mapRoot, FAttachmentTransformRules::KeepRelativeTransform);
        EntityPool[i]->SetActorRelativeScale3D(FVector(1.0f));
        EntityPool[i]->SetActorHiddenInGame(true);
        EntityPool[i]->SetActorEnableCollision(false);
        EntityPool[i]->SetActorTickEnabled(false);
        EntityPool[i]->setInUse(false);
    }

    firstAvailable = EntityPool[0];

    for (int i = 0; i < poolSize - 1; i++) {
        EntityPool[i]->setNext(EntityPool[i + 1]);
    }

    EntityPool[poolSize - 1]->setNext(nullptr);
}


APooledEntity* AObjectPoolActor::getAvailableEntity() 
{
    if (!firstAvailable) return nullptr;

    APooledEntity* newEntity = firstAvailable;

    firstAvailable = newEntity->getNext();
    turnOnEntity(newEntity);

    return newEntity;
}

void AObjectPoolActor::turnOnEntity(APooledEntity* entity) 
{
    entity->setInUse(true);
    entity->SetActorHiddenInGame(false);
    entity->SetActorEnableCollision(true);
    entity->SetActorTickEnabled(true);
}

void AObjectPoolActor::returnToPool(APooledEntity* entity) 
{
    entity->setOwnerSection(nullptr);
    entity->setInUse(false);
    entity->SetActorHiddenInGame(true);
    entity->SetActorEnableCollision(false);
    entity->SetActorTickEnabled(false);

    entity->setNext(firstAvailable);
    firstAvailable = entity;

}

