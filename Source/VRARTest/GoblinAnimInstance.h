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

    UFUNCTION()
    void AnimNotify_EnableAttack();

    UFUNCTION()
    void AnimNotify_DisableAttack();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    bool IsRunning = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    bool IsAttacking = false;

    void setOwningGoblin(AGoblinPooledEntity* entity)
    {
        OwningGoblin = entity;
    }

protected:
    UPROPERTY(BlueprintReadOnly)
    AGoblinPooledEntity* OwningGoblin;
};