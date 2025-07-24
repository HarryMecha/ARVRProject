// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinPooledEntity.h"
#include "VRCharacter.h"

AGoblinPooledEntity::AGoblinPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;
    maxHealth = 5;
    resetHealth();
    verticalHeartOffset = 45.0f;

    LeftHandCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandCollider"));
    LeftHandCollider->SetupAttachment(GetMesh(), TEXT("Left-Hand"));
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AGoblinPooledEntity::BeginPlay()
{
    Super::BeginPlay();

    if (LeftHandCollider)
    {
        LeftHandCollider->OnComponentBeginOverlap.AddDynamic(this, &AGoblinPooledEntity::OnLeftHandOverlap);
    }
}

void AGoblinPooledEntity::StartAttack() 
{
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGoblinPooledEntity::EndAttack()
{
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->PlayAnimation(idleAnimation, false);

}

void AGoblinPooledEntity::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        Cast<AVRCharacter>(OtherActor)->takeDamage(1.0f);
    }
}