// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapPooledEntity.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "VRCharacter.h"


ATrapPooledEntity::ATrapPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    boxCollider->SetupAttachment(RootComponent);

    entityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    entityMesh->SetupAttachment(boxCollider);

}

void ATrapPooledEntity::BeginPlay() 
{
    Super::BeginPlay();
    boxCollider->OnComponentBeginOverlap.AddDynamic(this, &ATrapPooledEntity::OnMeshBeginOverlap);

}

void ATrapPooledEntity::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()))
    {
        if (OtherComp->IsA(UCapsuleComponent::StaticClass()))
        {
            if (trapActivated == false)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("TrapActivated"));
                //apply random effect
                AVRCharacter* character = Cast<AVRCharacter>(OtherActor);
                //int randomIndex = FMath::RandRange(0,1);
                int randomIndex = 2;
                switch (randomIndex) {
                    case(0):
                    {
                        character->addPowerUp(EPowerUpType::HEALTHDOWN);
                        break;
                    }
                    case(1):
                    {
                        character->addPowerUp(EPowerUpType::ATTACKDOWN);
                        break;
                    }
                    case(2):
                    {
                        character->addPowerUp(EPowerUpType::SPEEDDOWN);
                        break;
                    }
                }
                //do that ghost thing
                trapActivated = true;
                boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                toggleMeshVisibility(true);
                entityFinished();
            }
        }

    }
}

void ATrapPooledEntity::resetTrap()
{
    boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    entityMesh->SetVisibility(true);
    trapActivated = false;
}

void ATrapPooledEntity::toggleTransparent(bool toggle)
{
    if (toggle && !isTransparent)
    {
        boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        isTransparent = true;
    }
    else if (!toggle && isTransparent)
    {
        boxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        isTransparent = false;
    }
}