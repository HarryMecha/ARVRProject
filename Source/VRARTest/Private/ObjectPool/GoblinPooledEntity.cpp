// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/GoblinPooledEntity.h"
#include "ObjectPool/GoblinAnimInstance.h"
#include "VRSpecific/VRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "ObjectPool/LivingEntityAIController.h"

AGoblinPooledEntity::AGoblinPooledEntity()
{
    PrimaryActorTick.bCanEverTick = true;
    maxHealth = 3;
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

    if (UGoblinAnimInstance* AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &AGoblinPooledEntity::OnAttackMontageEnded);
    }

}

void AGoblinPooledEntity::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    AIController = Cast<ALivingEntityAIController>(NewController);
}

void AGoblinPooledEntity::EnableAI()
{
    if (GetController() == nullptr)
    {
        ALivingEntityAIController* MyAIController = GetWorld()->SpawnActor<ALivingEntityAIController>();
        if (MyAIController)
        {
            MyAIController->Possess(this);
        }
    }
}

void AGoblinPooledEntity::StartAttack() 
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Attack Started")));
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGoblinPooledEntity::EndAttack()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Attack Ended")));
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGoblinPooledEntity::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        if (OtherComp->IsA(UCapsuleComponent::StaticClass())) {
            if (!playerBeenAttacked)
            {
                Cast<AVRCharacter>(OtherActor)->modifyHealth(-1.0f);

                playerBeenAttacked = true;
            }
        }
    }
    
}

void AGoblinPooledEntity::changeState(ELivingEntityState newState)
{
    currentState = newState;

    switch (currentState)
    {
    case ELivingEntityState::Idle:
        if (AIController)
        {
            AIController->StopChasingTarget();
        }
        PlayIdleAnimation();
        break;

    case ELivingEntityState::Chasing:
    {
        if (AIController && playerRef)
        {
            AIController->ChaseTarget(playerRef);
        }
        PlayRunAnimation();
    }
        break;

    case ELivingEntityState::Attacking:
        if (AIController)
        {
            AIController->StopChasingTarget();
        }
        PlayAttackAnimation();
        break;
    }
}

void AGoblinPooledEntity::PlayAttackAnimation()
{
    if (!isAttacking)
    {
        if (UGoblinAnimInstance* AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance()))
        {
            if (attackMontage && !AnimInstance->Montage_IsPlaying(attackMontage))
            {
               //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Playing Attacking Animation")));
                AnimInstance->IsRunning = false;
                AnimInstance->Montage_Play(attackMontage);
                isAttacking = true;
            }
        }
    }
}

void AGoblinPooledEntity::PlayIdleAnimation()
{
    if (UGoblinAnimInstance* AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Playing Idle Animation")));
        AnimInstance->IsRunning = false;
        AnimInstance->IsAttacking = false;
    }
}

void AGoblinPooledEntity::PlayRunAnimation()
{
    if (UGoblinAnimInstance* AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Playing Running Animation")));
        AnimInstance->IsRunning = true;
        AnimInstance->IsAttacking = false;
    }
}


void AGoblinPooledEntity::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == attackMontage)
    {
        isAttacking = false;
        playerBeenAttacked = false;

        if (playerInChaseCollider)
        {
            changeState(ELivingEntityState::Chasing);
        }
        else
        {
            changeState(ELivingEntityState::Idle);
        }
    }
}

void AGoblinPooledEntity::toggleTransparent(bool toggle)
{
    if (toggle == true && isTransparent == false)
    {
        GetMesh()->SetMaterial(0, transparentMaterial);
        attackRangeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        chaseRangeCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        isTransparent = true;
    }
    else if (toggle == false && isTransparent == true)
    {
        if (isFrenzied == true) {
            GetMesh()->SetMaterial(0, frenzyMaterial);
        }
        else if (isFrenzied == false)
        {
            GetMesh()->SetMaterial(0, regularMaterial);
        }
        attackRangeCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        chaseRangeCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        isTransparent = false;
    }
}