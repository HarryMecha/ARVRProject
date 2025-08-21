// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/VRPlayerUI.h"

void UVRPlayerUI::NativeConstruct()
{
    Super::NativeConstruct();
    UE_LOG(LogTemp, Warning, TEXT("UVRPlayerUI::NativeConstruct() fired"));

    if (dwarfHealthBar)
    {
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
    if (popUpText)
    {
        setPopUpText("Click On Connection Button");
        popUpText->SetVisibility(ESlateVisibility::Visible);
    }

}

void UVRPlayerUI::setPopUpText(FString text)
{
    popUpText->SetText(FText::FromString(text));
}