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
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Goblin Found")));

        OwningGoblin->StartAttack();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Goblin Not Found")));

    }
}

void UGoblinAnimInstance::AnimNotify_DisableAttack()
{
    if (OwningGoblin)
    {
        OwningGoblin->EndAttack();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Goblin Found")));
    }
}

