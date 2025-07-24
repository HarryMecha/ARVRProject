// Fill out your copyright notice in the Description page of Project Settings.


#include "LivingPooledEntity.h"
#include "MapSection.h"
#include "VRCharacter.h"

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
    attackRangeCollider = CreateDefaultSubobject<USphereComponent>(TEXT("AttackCollider"));
    attackRangeCollider->SetupAttachment(RootComponent);
    attackRangeCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ALivingPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    GetMesh()->PlayAnimation(idleAnimation, true);
    maxHealth = currentHealth;

    if (attackRangeCollider)
    {
        attackRangeCollider->OnComponentBeginOverlap.AddDynamic(this, &ALivingPooledEntity::OnAttackRangeOverlap);
    }
}

void ALivingPooledEntity::takeDamage(float amount)
{
	currentHealth -= amount;
    float currentHealthAmount = currentHealth;

    for (int i = heartActors.Num()-1; i >= 0; i--)
    {
        if (currentHealthAmount >= 1.0f)
        {
            heartActors[i]->setHeartState(EHeartFillState::FULL);
            currentHealthAmount -= 1.0f;
        }
        else
        {
            if (currentHealthAmount == 0.25f)
            {
                heartActors[i]->setHeartState(EHeartFillState::QUARTER);
                currentHealthAmount = 0.0f;
            }
            else if (currentHealthAmount == 0.5f)
            {
                heartActors[i]->setHeartState(EHeartFillState::HALF);
                currentHealthAmount = 0.0f;
            }
            else if (currentHealthAmount == 0.75f)
            {
                heartActors[i]->setHeartState(EHeartFillState::THREEQUARTER);
                currentHealthAmount = 0.0f;
            }
            else
            {
                heartActors[i]->setHeartState(EHeartFillState::EMPTY);
            }
        }
    }
    

	if (currentHealth <= 0.0f)
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
            heart->setHeartState(EHeartFillState::FULL);
        }
    }
}

void ALivingPooledEntity::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        GetMesh()->PlayAnimation(attackAnimation, false);
    }
}