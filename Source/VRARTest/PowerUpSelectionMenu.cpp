// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpSelectionMenu.h"
#include "Components/Image.h"

void UPowerUpSelectionMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	imageArray.Add(SpeedOptionImage);
	imageMap.Add(SpeedOptionImage, SpeedOptionActivated);
	explanationMap.Add(SpeedOptionImage, SpeedExplanationImage);
	
	imageArray.Add(PotionOptionImage);
	imageMap.Add(PotionOptionImage, PotionOptionActivated);
	explanationMap.Add(PotionOptionImage, PotionExplanationImage);


	imageArray.Add(CancelOptionImage);
	imageMap.Add(CancelOptionImage, CancelOptionActivated);
	explanationMap.Add(CancelOptionImage, CancelExplanationImage);

	
	imageArray.Add(LanternOptionImage);
	imageMap.Add(LanternOptionImage, LanternOptionActivated);
	explanationMap.Add(LanternOptionImage, LanternExplanationImage);


	imageArray.Add(AttackOptionImage);
	imageMap.Add(AttackOptionImage, AttackOptionActivated);
	explanationMap.Add(AttackOptionImage, AttackExplanationImage);


	AttackExplanationImage->SetVisibility(ESlateVisibility::Hidden);
	SpeedExplanationImage->SetVisibility(ESlateVisibility::Hidden);
	PotionExplanationImage->SetVisibility(ESlateVisibility::Hidden);
	LanternExplanationImage->SetVisibility(ESlateVisibility::Hidden);
	CancelExplanationImage->SetVisibility(ESlateVisibility::Hidden);

}

void UPowerUpSelectionMenu::reloadMenu(int noHammer, int noSpeed, int noPotion, int noLanten)
{
	if (noHammer >= 1)
	{
		AttackOptionImage-> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[AttackOptionImage] = true;
		explanationMap[AttackOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		AttackOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[AttackOptionImage] = false;
		explanationMap[AttackOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	if (noSpeed >= 1)
	{
		SpeedOptionImage -> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[SpeedOptionImage] = true;
		explanationMap[SpeedOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SpeedOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[SpeedOptionImage] = false;
		explanationMap[SpeedOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	if (noPotion >= 1)
	{
		PotionOptionImage -> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[PotionOptionImage] = true;
		explanationMap[PotionOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		PotionOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[PotionOptionImage] = false;
		explanationMap[PotionOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	if (noLanten >= 1)
	{
		LanternOptionImage-> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[LanternOptionImage] = true;
		explanationMap[LanternOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		LanternOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[LanternOptionImage] = false;
		explanationMap[LanternOptionImage]->SetVisibility(ESlateVisibility::Hidden);
	}
	CancelOptionImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
	explanationMap[CancelOptionImage]->SetVisibility(ESlateVisibility::Hidden);
}

void UPowerUpSelectionMenu::highlightSlice(int index)
{
	if (index >= 0) {
		UImage* newImage = imageArray[index];

		if (*imageMap.Find(newImage) == false)
		{
			return;
		}

		if (highlightedImage && highlightedImage != newImage)
		{
			bool* wasActive = imageMap.Find(highlightedImage);
			if (wasActive && *wasActive)
			{
				highlightedImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
				UImage* highlightedExplanation = *explanationMap.Find(highlightedImage);
				highlightedExplanation->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		highlightedImage = newImage;
		highlightedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		UImage* highlightedExplanation = *explanationMap.Find(highlightedImage);
		highlightedExplanation->SetVisibility(ESlateVisibility::Visible);
	}
}