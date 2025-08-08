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
    chaseRangeCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseCollider"));
    chaseRangeCollider->SetupAttachment(RootComponent);
    chaseRangeCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ALivingPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    maxHealth = currentHealth;

    if (attackRangeCollider)
    {
        attackRangeCollider->OnComponentBeginOverlap.AddDynamic(this, &ALivingPooledEntity::OnAttackRangeOverlap);
        attackRangeCollider->OnComponentEndOverlap.AddDynamic(this, &ALivingPooledEntity::OnAttackRangeExitOverlap);

    }
    if (chaseRangeCollider)
    {
        chaseRangeCollider->OnComponentBeginOverlap.AddDynamic(this, &ALivingPooledEntity::OnChaseRangeOverlap);
        chaseRangeCollider->OnComponentEndOverlap.AddDynamic(this, &ALivingPooledEntity::OnChaseRangeExitOverlap);

    }
   
    currentState = ELivingEntityState::Idle;
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
    DestroyHealthUI();
    AMapSection* ownerSection = getPoolInterface()->getOwnerSection();
    if (ownerSection) {
        ownerSection->interactionConclusion();
    }
}

void ALivingPooledEntity::CreateHealthUI()
{
    if (!heartActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("heartActorClass is null. Skipping health UI creation."));
        return;
    }

    float spacing = 20.0f;

    for (int i = 0; i < maxHealth; i++)
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

void ALivingPooledEntity::DestroyHealthUI()
{
    for (AHeartActor* Heart : heartActors)
    {
        if (Heart)
        {
            Heart->Destroy();
        }
    }
    heartActors.Empty();
}

void ALivingPooledEntity::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        changeState(ELivingEntityState::Attacking);
        playerInAttackCollider = true;
    }
}

void ALivingPooledEntity::OnAttackRangeExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        if (!isAttacking) {
            changeState(ELivingEntityState::Chasing);
        }
        playerInAttackCollider = false;
    }
}

void ALivingPooledEntity::OnChaseRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        playerRef = OtherActor;

        if (!isAttacking) {
            changeState(ELivingEntityState::Chasing);
        }
        playerInChaseCollider = true;

    }
}

void ALivingPooledEntity::OnChaseRangeExitOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        if (OtherActor == playerRef)
        {
            playerRef = nullptr;
        }
        changeState(ELivingEntityState::Idle);
        playerInChaseCollider = false;
    }
}