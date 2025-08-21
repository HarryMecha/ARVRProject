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

	//reloadMenu() checks each of the ints in it's parameter if any of them are greater than 1 they have their opacity and colour
	// set higher to indicate they are accessible, if the int is 0 it will change that to darker colours and will no longer be selectable
	// due to it's imageMap boolean being set to false.
	void reloadMenu(int noHammer, int noSpeed, int noPotion, int noLanten);

	//highlightSlice() takes in an int this is based on the current positon the left motion controller stick converted into a number between 0 and 5, this corresponds
	// to the index numbers of the explanation map, if the index number matches one of the indexes of an entry in the imageMap where the boolean is true an explanation will be 
	// displayed and the button will be highlighted.
	void highlightSlice(int index);

	UImage* highlightedImage = nullptr;

};
