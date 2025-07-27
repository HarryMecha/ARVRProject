// Fill out your copyright notice in the Description page of Project Settings.


#include "GoblinAnimInstance.h"
#include "GameFramework/Actor.h"
#include "GoblinPooledEntity.h"

void UGoblinAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    AActor* ownerActor = GetOwningActor();
    if (ownerActor)
    {
        OwningGoblin = Cast<AGoblinPooledEntity>(ownerActor);
    }
   /*
    if (!IsValid(OwningGoblin))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Goblin Not Valid")));
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("Goblin is Valid")));

    }
    */
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

