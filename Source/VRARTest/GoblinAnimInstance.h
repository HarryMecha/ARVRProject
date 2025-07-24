// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GoblinPooledEntity.h"
#include "GoblinAnimInstance.generated.h"

UCLASS()
class VRARTEST_API UGoblinAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;

    UFUNCTION(BlueprintCallable)
    void AnimNotify_EnableAttack();

    UFUNCTION(BlueprintCallable)
    void AnimNotify_DisableAttack();

protected:
    UPROPERTY(BlueprintReadOnly)
    AGoblinPooledEntity* OwningGoblin;
};