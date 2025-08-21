// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/VRMapWidget.h"
#include "Components/ScaleBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void UVRMapWidget::NativeConstruct()
{
    Super::NativeConstruct();
    TArray<UWidget*> canvasChildren = MapCanvasPanel->GetAllChildren();
    MarkerArrowImage->SetVisibility(ESlateVisibility::Hidden);
    for (UWidget* child : canvasChildren)
    {
        if (child->IsA(UScaleBox::StaticClass()))
        {
            UScaleBox* mapSectionBox = Cast<UScaleBox>(child);
            UWidget* mapSectionChild = mapSectionBox->GetChildAt(0);
            if (mapSectionChild->IsA(UVRMapSectionWidget::StaticClass()))
            {
                UVRMapSectionWidget* mapSectionWidget = Cast<UVRMapSectionWidget>(mapSectionChild);
                mapSectionWidget->changeCrossVisibility(false);
                mapSectionImages.Add(mapSectionWidget);
            }
        }
    }
    sortMapSections();
}


void UVRMapWidget::sortMapSections()
{
    mapSectionImages.Sort([](UVRMapSectionWidget& A, UVRMapSectionWidget& B)
        {
            return A.indexNumber < B.indexNumber;
        });
}


UVRMapSectionWidget* UVRMapWidget::getMapSectionFromArray(int index)
{
    return mapSectionImages[index];
}

void UVRMapWidget::changeMarkerVisibility(bool toggle)
{
    if (toggle == true)
    {
        MarkerArrowImage->SetVisibility(ESlateVisibility::Visible);
    }
    if (toggle == false)
    {
        MarkerArrowImage->SetVisibility(ESlateVisibility::Hidden);

    }
}

void UVRMapWidget::updatePlayerMarker(int targetIndex)
{
    UCanvasPanelSlot* MarkerSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(MarkerArrowImage->GetParent());
    UCanvasPanelSlot* TargetSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(mapSectionImages[targetIndex]->GetParent());

    const FVector2D TargetPosition = TargetSlot->GetPosition();

    MarkerSlot->SetPosition(TargetPosition);

}

void UVRMapWidget::updatePlayerMarkerRotation(FRotator playerRotation)
{
    MarkerArrowImage->SetRenderTransformAngle(playerRotation.Yaw + 180);
}