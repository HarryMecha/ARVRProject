// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
class UImage;
#include "PowerUpSelectionMenu.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UPowerUpSelectionMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AttackOptionImage;

	bool AttackOptionActivated = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* SpeedOptionImage;

	bool SpeedOptionActivated = false;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* PotionOptionImage;

	bool PotionOptionActivated = false;


	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* CancelOptionImage;

	bool CancelOptionActivated = true;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* LanternOptionImage;

	bool LanternOptionActivated = false;


	TArray<UImage*> imageArray;

	TMap<UImage*, bool> imageMap;

	void reloadMenu(int noHammer, int noSpeed, int noPotion, int noLanten);

	void highlightSlice(int index);

	UImage* highlightedImage = nullptr;

};
