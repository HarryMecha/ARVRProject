// Fill out your copyright notice in the Description page of Project Settings.


#include "LivingPooledEntity.h"
#include "MapSection.h"

ALivingPooledEntity::ALivingPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    poolComponent = CreateDefaultSubobject<UPooledEntityComponent>(TEXT("PooledEntityComponent"));

    // Create and attach mesh
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetEnableGravity(true);
}

void ALivingPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    GetMesh()->PlayAnimation(idleAnimation, true);
    maxHealth = currentHealth;
}

void ALivingPooledEntity::takeDamage(float amount)
{
	currentHealth -= amount;

	if (currentHealth <= 0.f)
	{
		Die();
	}
}

// Handle death
void ALivingPooledEntity::Die()
{
    AMapSection* ownerSection = getPoolInterface()->getOwnerSection();
    if (ownerSection) {
        ownerSection->interactionConclusion(this);
    }
    resetHealth();
}