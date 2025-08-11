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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* AttackExplanationImage;

	bool AttackOptionActivated = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* SpeedOptionImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* SpeedExplanationImage;

	bool SpeedOptionActivated = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* PotionOptionImage;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* PotionExplanationImage;
	
	bool PotionOptionActivated = false;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* CancelOptionImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* CancelExplanationImage;

	bool CancelOptionActivated = true;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* LanternOptionImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* LanternExplanationImage;

	bool LanternOptionActivated = false;

	TArray<UImage*> imageArray;

	TMap<UImage*, bool> imageMap;

	TMap<UImage*, UImage*> explanationMap;


	void reloadMenu(int noHammer, int noSpeed, int noPotion, int noLanten);

	void highlightSlice(int index);

	UImage* highlightedImage = nullptr;

};
