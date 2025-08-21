// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageFlashWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UDamageFlashWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //PlayFlash() plays the flash animation of the damage widget when a player takes damage.
    UFUNCTION(BlueprintCallable)
    void PlayFlash();

protected:
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Flash;
};
