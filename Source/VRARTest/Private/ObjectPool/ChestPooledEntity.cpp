// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/ChestPooledEntity.h"
#include "Map/MapSection.h"
#include "VRSpecific/VRCharacter.h"
#include "VRSpecific/PowerUpActor.h"

AChestPooledEntity::AChestPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    boxCollider->SetupAttachment(RootComponent);

    // Create and attach mesh
    entitySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    entitySkeletalMesh->SetupAttachment(boxCollider);

    powerUpHolder = CreateDefaultSubobject<USceneComponent>(TEXT("PowerUpSpawnPoint"));
    powerUpHolder->SetupAttachment(RootComponent);
}


void AChestPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    boxCollider->OnComponentBeginOverlap.AddDynamic(this, &AChestPooledEntity::OnMeshBeginOverlap);

    entitySkeletalMesh->SetMaterial(0, normalMaterial);

    dynamicMaterial = entitySkeletalMesh->CreateAndSetMaterialInstanceDynamic(0);

    TArray<AActor*> childActors;
    GetAllChildActors(childActors);

    for (AActor* child : childActors)
    {
        APowerUpActor* powerUp = Cast<APowerUpActor>(child);
        if (IsValid(powerUp))
        {
            powerUpActors.Add(powerUp);

        }
    }

    for (int32 i = 0; i < powerUpActors.Num(); i++)
    {
        APowerUpActor* powerUp = powerUpActors[i];
        if (!IsValid(powerUp))
        {
            powerUpActors.Remove(powerUp);
        }
    }
}

void AChestPooledEntity::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (fadeOut)
    {
        boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        fadeTimer += DeltaTime;

        if (currentOpacity > 0.0f)
        {
            currentOpacity = FMath::Clamp(1.0f - (fadeTimer / maxFadeTime), 0.0f, 1.0f);

            dynamicMaterial->SetScalarParameterValue(FName("Opacity"), currentOpacity);
        }
        else
        {
            fadeOut = false;
            entitySkeletalMesh->SetVisibility(false);
            if (IsValid(selectedPowerUp))
            {
                selectedPowerUp->adjustCollision();
            }
        }
    }
    if (powerUpPickedUp)
    {
        entityFinished();
        powerUpPickedUp = false;
    }

}

void AChestPooledEntity::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
    if (OtherComp->ComponentHasTag("Hand")) {

        if (!animationPlayed)
        {
            entitySkeletalMesh->PlayAnimation(openAnimation, false);
            animationPlayed = true;
            fadeOut = true;
        }
        if (!chestOpened)
        {
            int32 randomIndex = FMath::RandRange(0, powerUpActors.Num() - 1);
            selectedPowerUp = powerUpActors[randomIndex];

            if (IsValid(selectedPowerUp))
            {
                selectedPowerUp->setVisible();
            }
            chestOpened = true;
        }
    }
}

void AChestPooledEntity::resetChest()
{
    boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    entitySkeletalMesh->SetVisibility(true);
    fadeTimer = 0.0f;
    currentOpacity = 1.0f;
    dynamicMaterial->SetScalarParameterValue(FName("Opacity"), currentOpacity);
    animationPlayed = false;
    chestOpened = false;
    powerUpPickedUp = false;
    selectedPowerUp = nullptr;
    entitySkeletalMesh->Stop();
    entitySkeletalMesh->SetAnimation(nullptr);

}

void AChestPooledEntity::toggleTransparent(bool toggle)
{
    if (toggle && !isTransparent)
    {
        entitySkeletalMesh->SetMaterial(0, transparentMaterial);
        boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        isTransparent = true;
    }
    else if (!toggle && isTransparent)
    {
        entitySkeletalMesh->SetMaterial(0, normalMaterial);
        dynamicMaterial = entitySkeletalMesh->CreateAndSetMaterialInstanceDynamic(0);
        boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        isTransparent = false;
    }
}