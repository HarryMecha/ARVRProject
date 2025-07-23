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
    heartRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeartRoot"));
    heartRoot->SetupAttachment(RootComponent);
    heartRoot->SetRelativeLocation(FVector(0, 0, verticalHeartOffset));
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
    float Amount = amount;

    while (Amount > 1)
    {
       // for(int i = heartActors)
    }

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

void ALivingPooledEntity::CreateHealthUI()
{
    if (!heartActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("heartActorClass is null. Skipping health UI creation."));
        return;
    }

    float spacing = 20.0f;

    for (int i = 0; i < maxHealth; ++i)
    {
        float horizontalOffset = (i - (maxHealth - 1) / 2.0f) * spacing;

        FVector spawnLocation = GetActorLocation() + FVector(0, horizontalOffset, verticalHeartOffset);
        FRotator spawnRotation = FRotator(0.0f, 90.0f, 0.0f);

        AHeartActor* heart = GetWorld()->SpawnActor<AHeartActor>(heartActorClass, spawnLocation, spawnRotation);
        if (heart)
        {
            heart->AttachToComponent(heartRoot, FAttachmentTransformRules::KeepWorldTransform);
            heartActors.Add(heart);
            heart->SetHeartState(EHeartFillState::FULL);
        }
    }
}