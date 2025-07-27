// Fill out your copyright notice in the Description page of Project Settings.


#include "ChestPooledEntity.h"
#include "MapSection.h"
#include "VRCharacter.h"

AChestPooledEntity::AChestPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    RootComponent = boxCollider;

    // Create and attach mesh
    entitySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    entitySkeletalMesh->SetupAttachment(RootComponent);

}


void AChestPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    boxCollider->OnComponentBeginOverlap.AddDynamic(this, &AChestPooledEntity::OnMeshBeginOverlap);

    dynamicMaterial = entitySkeletalMesh->CreateAndSetMaterialInstanceDynamic(0);
}

void AChestPooledEntity::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (fadeOut)
    {
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
            boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
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

        /*
        AMapSection* ownerSection = getPoolInterface()->getOwnerSection();
        if (ownerSection) {
            ownerSection->interactionConclusion(this);
        }
        */
    }
}