// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpBarWidget.h"
#include "Components/HorizontalBoxSlot.h"

void UPowerUpBarWidget::CreatePowerUpBar()
{
    textureTypeMap.Empty();

    textureTypeMap.Add(EPowerUpType::ATTACK, AttackUpImage);
    textureTypeMap.Add(EPowerUpType::SPEED, SpeedUpImage);
    textureTypeMap.Add(EPowerUpType::SPEEDDOWN, SpeedDownImage);
    textureTypeMap.Add(EPowerUpType::NONE, clearImage);


    powerUpBar->ClearChildren();

    for (int32 i = 0; i < 5; i++)
    {
        UImage* newImage = NewObject<UImage>(this, UImage::StaticClass());
        powerUpBar->AddChild(newImage);

        if (UHorizontalBoxSlot* slot = Cast<UHorizontalBoxSlot>(newImage->Slot))
        {
            slot->SetPadding(FMargin(5));
            slot->SetHorizontalAlignment(HAlign_Center);
            slot->SetVerticalAlignment(VAlign_Center);
        }
        powerUpSlots.Add(EPowerUpType::NONE);
        powerUpImages.Add(newImage);
        newImage->SetBrushFromTexture(*textureTypeMap.Find(powerUpSlots[i]));
    }
}

void UPowerUpBarWidget::addPowerUp(EPowerUpType type)
{
    for (int32 i = 0; i < powerUpSlots.Num(); i++)
    {
       
        if (powerUpSlots[i] == EPowerUpType::NONE)
        {
            powerUpSlots[i] = type;
            
            powerUpImages[i]->SetBrushFromTexture(*textureTypeMap.Find(type));
            
            return;
        }
    }
}


void UPowerUpBarWidget::removePowerUp(EPowerUpType type)
{
    for (int32 i = 0; i < powerUpSlots.Num(); i++)
    {
        if (powerUpSlots[i] == type)
        {
            for (int32 j = i; j < powerUpSlots.Num() - 1; j++)
            {
                powerUpSlots[j] = powerUpSlots[j + 1];
                powerUpImages[j]->SetBrushFromTexture(*textureTypeMap.Find(powerUpSlots[j]));
            }

            powerUpSlots[powerUpSlots.Num() - 1] = EPowerUpType::NONE;
            powerUpImages[powerUpSlots.Num() - 1]->SetBrushFromTexture(*textureTypeMap.Find(EPowerUpType::NONE));

            return;
        }
    }
}