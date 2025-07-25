// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinPooledEntity.h"
#include "GoblinAnimInstance.h"
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

    if (UGoblinAnimInstance* AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        AnimInstance->OnMontageEnded.AddDynamic(this, &AGoblinPooledEntity::OnAttackMontageEnded);
    }
}

void AGoblinPooledEntity::StartAttack() 
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Attack Started")));
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGoblinPooledEntity::EndAttack()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Attack Ended")));
    LeftHandCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AGoblinPooledEntity::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
    {
        if (!playerBeenAttacked) 
        {
            Cast<AVRCharacter>(OtherActor)->takeDamage(1.0f);
            playerBeenAttacked = true;
        }
    }
}

void AGoblinPooledEntity::changeState(ELivingEntityState newState)
{
    currentState = newState;

    switch (currentState)
    {
    case ELivingEntityState::Idle:
        //StopMovement();
        PlayIdleAnimation();
        break;

    case ELivingEntityState::Chasing:
       //MoveToPlayer();
        PlayRunAnimation();
        break;

    case ELivingEntityState::Attacking:
        //StopMovement();
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
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Playing Attacking Animation")));
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
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Playing Idle Animation")));
        AnimInstance->IsRunning = false;
        AnimInstance->IsAttacking = false;
    }
}

void AGoblinPooledEntity::PlayRunAnimation()
{
    if (UGoblinAnimInstance* AnimInstance = Cast<UGoblinAnimInstance>(GetMesh()->GetAnimInstance()))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Playing Running Animation")));
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