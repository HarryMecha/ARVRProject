// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpSelectionMenu.h"
#include "Components/Image.h"

void UPowerUpSelectionMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	imageArray.Add(SpeedOptionImage);
	imageMap.Add(SpeedOptionImage, SpeedOptionActivated);
	
	imageArray.Add(PotionOptionImage);
	imageMap.Add(PotionOptionImage, PotionOptionActivated);
	
	imageArray.Add(CancelOptionImage);
	imageMap.Add(CancelOptionImage, CancelOptionActivated);

	
	imageArray.Add(LanternOptionImage);
	imageMap.Add(LanternOptionImage, LanternOptionActivated);
	
	imageArray.Add(AttackOptionImage);
	imageMap.Add(AttackOptionImage, AttackOptionActivated);


}

void UPowerUpSelectionMenu::reloadMenu(int noHammer, int noSpeed, int noPotion, int noLanten)
{
	if (noHammer >= 1)
	{
		AttackOptionImage-> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[AttackOptionImage] = true;
	}
	else
	{
		AttackOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[AttackOptionImage] = false;
	}
	if (noSpeed >= 1)
	{
		SpeedOptionImage -> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[SpeedOptionImage] = true;
	}
	else
	{
		SpeedOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[SpeedOptionImage] = false;
	}
	if (noPotion >= 1)
	{
		PotionOptionImage -> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[PotionOptionImage] = true;
	}
	else
	{
		PotionOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[PotionOptionImage] = false;
	}
	if (noLanten >= 1)
	{
		LanternOptionImage-> SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
		imageMap[LanternOptionImage] = true;
	}
	else
	{
		LanternOptionImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		imageMap[LanternOptionImage] = false;
	}
	CancelOptionImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.75f));
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
			}
		}

		highlightedImage = newImage;
		highlightedImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
}