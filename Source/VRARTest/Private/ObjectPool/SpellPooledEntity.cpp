// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/SpellPooledEntity.h"
#include "Components/BoxComponent.h"
#include "VRSpecific/VRCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"



ASpellPooledEntity::ASpellPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
    boxCollider->SetupAttachment(RootComponent);

    entityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    entityMesh->SetupAttachment(boxCollider);

    timeToLive = 2.0f;

    projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    projectileMovement->InitialSpeed = 750.0f; 
    projectileMovement->MaxSpeed = 750.0f;
    projectileMovement->bRotationFollowsVelocity = true;
    projectileMovement->ProjectileGravityScale = 0.0f;
    projectileMovement->bAutoActivate = false;
}


void ASpellPooledEntity::BeginPlay()
{
    Super::BeginPlay();

    if (boxCollider)
    {
        boxCollider->OnComponentBeginOverlap.AddDynamic(this, &ASpellPooledEntity::OnColliderOverlap);
    }
    projectileMovement->Deactivate();
}

void ASpellPooledEntity::startTimeToLive()
{
    if (GetWorldTimerManager().IsTimerActive(timerHandle))
    {
        GetWorldTimerManager().ClearTimer(timerHandle);
    }

    if (timeToLive > 0.0f)
    {
        GetWorldTimerManager().SetTimer(timerHandle,this,&ASpellPooledEntity::onTimeToLiveExpired, timeToLive,false);
    }
}

void ASpellPooledEntity::onTimeToLiveExpired()
{
    manager->nonPlacableObjectConclusion(this);
}

void ASpellPooledEntity::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        if (OtherComp->IsA(UCapsuleComponent::StaticClass())) {

                Cast<AVRCharacter>(OtherActor)->modifyHealth(-1.0f);

                onTimeToLiveExpired();
        }
    }
    else if (OtherActor->Tags.Contains("VRRep") && OtherComp->IsA(UCapsuleComponent::StaticClass()))
    {
        onTimeToLiveExpired();
    }
}