// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/GoblinAnimInstance.h"
#include "GameFramework/Actor.h"
#include "ObjectPool/GoblinPooledEntity.h"

void UGoblinAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    AActor* ownerActor = GetOwningActor();
    if (ownerActor)
    {
        OwningGoblin = Cast<AGoblinPooledEntity>(ownerActor);
    }
}

void UGoblinAnimInstance::AnimNotify_EnableAttack()
{

    if (IsValid(OwningGoblin))
    {
        OwningGoblin->StartAttack();
    }

  
}

void UGoblinAnimInstance::AnimNotify_DisableAttack()
{
    if (IsValid(OwningGoblin))
    {
        OwningGoblin->EndAttack();
    }

}

