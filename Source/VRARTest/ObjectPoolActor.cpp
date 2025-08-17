// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolActor.h"
#include "PooledEntityInterface.h"
#include "PooledEntityComponent.h"
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

void AObjectPoolActor::initialisePool(TSubclassOf<AActor> poolTypeClass)
{
    PooledEntityClass = poolTypeClass;
    TArray<AActor*> AllActors;
    AActor* mapRoot = nullptr;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
    for (AActor* actor : AllActors)
    {
        if (actor->ActorHasTag("MapRoot"))
        {
            mapRoot = actor;
        }
    }


    EntityPool.SetNum(poolSize);
    
    for (int i = 0; i < poolSize; i++) {
        FActorSpawnParameters spawnParams;
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
        AActor* poolMember = GetWorld()->SpawnActor<AActor>(PooledEntityClass, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);

        poolMember->AttachToActor(mapRoot, FAttachmentTransformRules::KeepRelativeTransform);
        poolMember->SetActorRelativeScale3D(FVector(1.0f));
        poolMember->SetActorHiddenInGame(true);
        poolMember->SetActorEnableCollision(false);
        poolMember->SetActorTickEnabled(false);
       
        UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(poolMember->GetComponentByClass(UPooledEntityComponent::StaticClass()));
        pooledComponent->setInUse(false);
        pooledComponent->setNext(nullptr);
        pooledComponent->setOwnerSection(nullptr);


        EntityPool[i] = poolMember;
    }
    
    for (int i = 0; i < poolSize - 1; i++)
    {
       UPooledEntityComponent* currentPoolComponent = Cast<UPooledEntityComponent>(EntityPool[i]->GetComponentByClass(UPooledEntityComponent::StaticClass()));
       UPooledEntityComponent* nextPoolComponent = Cast<UPooledEntityComponent>(EntityPool[i+1]->GetComponentByClass(UPooledEntityComponent::StaticClass()));
       currentPoolComponent->setNext(nextPoolComponent);
    }

    UPooledEntityComponent* poolComponent = Cast<UPooledEntityComponent>(EntityPool[poolSize - 1]->GetComponentByClass(UPooledEntityComponent::StaticClass()));
    if (poolComponent) {
        poolComponent->setNext(nullptr);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Last entity in pool (%s) does not have a UPooledEntityComponent!"), *EntityPool[poolSize - 1]->GetName());
    }

    firstAvailable = EntityPool[0];
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Pool Initialised")));

}


AActor* AObjectPoolActor::getAvailableEntity()
{
    if (!firstAvailable)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("firstAvailable not working")));

        return nullptr;
    }
    

    AActor* newEntity = firstAvailable;
    UPooledEntityComponent* poolComponent = Cast<UPooledEntityComponent>(newEntity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
    firstAvailable = Cast<AActor>(poolComponent->getNext()->GetOwner());
    turnOnEntity(newEntity);

    return newEntity;
}

void AObjectPoolActor::turnOnEntity(AActor* entity) 
{
    UPooledEntityComponent* poolComponent = Cast<UPooledEntityComponent>(entity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
    poolComponent->setInUse(true);
    entity->SetActorHiddenInGame(false);
    entity->SetActorEnableCollision(true);
    entity->SetActorTickEnabled(true);
}

void AObjectPoolActor::returnToPool(AActor* entity)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Trying to return to the pool")));

 
    UPooledEntityComponent* poolComponent = Cast<UPooledEntityComponent>(entity->GetComponentByClass(UPooledEntityComponent::StaticClass()));
    poolComponent->setOwnerSection(nullptr);
    poolComponent->setInUse(false);
   
    if (firstAvailable) {
        UPooledEntityComponent* firstAvailableComponent = Cast<UPooledEntityComponent>(firstAvailable->GetComponentByClass(UPooledEntityComponent::StaticClass()));
        poolComponent->setNext(firstAvailableComponent);
    }
    else {
        poolComponent->setNext(nullptr);
    }

    entity->SetActorHiddenInGame(true);
    entity->SetActorEnableCollision(false);
    entity->SetActorTickEnabled(false);

    firstAvailable = entity;
 
}

