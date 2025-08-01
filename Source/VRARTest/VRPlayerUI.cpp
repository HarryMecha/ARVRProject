// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayerUI.h"

void UVRPlayerUI::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("UVRPlayerUI::NativeConstruct() fired"));

    if (dwarfHealthBar)
    {
        UE_LOG(LogTemp, Warning, TEXT("dwarfHealthBar is valid"));
        dwarfHealthBar->setHealthBarHeaderText("Dwarf Health:");
        dwarfHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("dwarfHealthBar is NULL"));
    }
    if (FrozenImage)
    {
        FrozenImage->SetVisibility(ESlateVisibility::Hidden);
    }


}
