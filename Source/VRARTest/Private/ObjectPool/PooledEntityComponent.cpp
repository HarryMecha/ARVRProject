// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/PooledEntityComponent.h"

// Sets default values for this component's properties
UPooledEntityComponent::UPooledEntityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPooledEntityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UPooledEntityComponent::setInUse(bool isInUse) 
{ 
	inUse = isInUse;
}

bool UPooledEntityComponent::getInUse() const 
{ 
	return inUse;
}

void UPooledEntityComponent::setNext(UPooledEntityComponent* entity)
{ 
	nextEntity = entity; 
}

UPooledEntityComponent* UPooledEntityComponent::getNext() const
{ 
	return nextEntity; 
}

void UPooledEntityComponent::setOwnerSection(AMapSection* section) 
{ 
	ownerSection = section; 
}

AMapSection* UPooledEntityComponent::getOwnerSection() const 
{ 
	return ownerSection; 
}
