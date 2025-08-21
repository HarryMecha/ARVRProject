// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/VRMapSectionWidget.h"

void UVRMapSectionWidget::changeCrossVisibility(bool toggle)
{
	if (toggle == true)
	{
		CrossImage->SetVisibility(ESlateVisibility::Visible);
	}
	if (toggle == false)
	{
		CrossImage->SetVisibility(ESlateVisibility::Hidden);

	}
}
