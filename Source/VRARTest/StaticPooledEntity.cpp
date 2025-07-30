// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticPooledEntity.h"
#include "MapSection.h"

// Sets default values
AStaticPooledEntity::AStaticPooledEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	poolComponent = CreateDefaultSubobject<UPooledEntityComponent>(TEXT("PooledEntityComponent"));


}

// Called when the game starts or when spawned
void AStaticPooledEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStaticPooledEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStaticPooledEntity::entityFinished()
{
	AMapSection* ownerSection = getPoolInterface()->getOwnerSection();
	if (ownerSection) {
		ownerSection->interactionConclusion();
	}
}

