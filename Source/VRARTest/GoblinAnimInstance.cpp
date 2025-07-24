// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinAnimInstance.h"
#include "GameFramework/Actor.h"
#include "GoblinPooledEntity.h"

void UGoblinAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwningGoblin = Cast<AGoblinPooledEntity>(TryGetPawnOwner());
}

void UGoblinAnimInstance::AnimNotify_EnableAttack()
{
    if (OwningGoblin)
    {
        OwningGoblin->StartAttack();
    }
}

void UGoblinAnimInstance::AnimNotify_DisableAttack()
{
    if (OwningGoblin)
    {
        OwningGoblin->EndAttack();
    }
}

