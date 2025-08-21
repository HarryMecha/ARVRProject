// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HeartWidget.h"


void UHeartWidget::NativeConstruct()
{
	Super::NativeConstruct();
    setHeartState(EHeartFillState::FULL);
}

void UHeartWidget::setHeartState(EHeartFillState newState)
{
    switch (newState)
    {
    case EHeartFillState::FULL:
        heartImage->SetBrushFromTexture(fullHeartTexture);
        currentState = EHeartFillState::FULL;
        break;
    case EHeartFillState::THREEQUARTER:
        heartImage->SetBrushFromTexture(threeQuarterHeartTexture);
        currentState = EHeartFillState::THREEQUARTER;
        break;
    case EHeartFillState::HALF:
        heartImage->SetBrushFromTexture(halfHeartTexture);
        currentState = EHeartFillState::HALF;
        break;
    case EHeartFillState::QUARTER:
        heartImage->SetBrushFromTexture(quarterHeartTexture);
        currentState = EHeartFillState::QUARTER;
        break;
    case EHeartFillState::EMPTY:
        heartImage->SetBrushFromTexture(emptyHeartTexture);
        currentState = EHeartFillState::EMPTY;
        break;
    }
}