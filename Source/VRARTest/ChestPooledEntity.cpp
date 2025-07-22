// Fill out your copyright notice in the Description page of Project Settings.


#include "ChestPooledEntity.h"
#include "MapSection.h"
#include "VRCharacter.h"

AChestPooledEntity::AChestPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // Create and attach mesh
    entityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    entityMesh->SetupAttachment(RootComponent);
}


void AChestPooledEntity::BeginPlay()
{
    Super::BeginPlay();
    entityMesh->OnComponentBeginOverlap.AddDynamic(this, &AChestPooledEntity::OnMeshBeginOverlap);

}

void AChestPooledEntity::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        AMapSection* ownerSection = getPoolInterface()->getOwnerSection();
        if (ownerSection) {
            ownerSection->interactionConclusion(this);
        }
    }
}