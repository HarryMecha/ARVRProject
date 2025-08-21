// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "VRMapSectionWidget.generated.h"

/**
 * This is just the setup class so the Results Screen Widget is visible in the blueprint, it contains little functionality only declerations.
 */

UCLASS()
class VRARTEST_API UVRMapSectionWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	void changeCrossVisibility(bool toggle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* MapSectionImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* CrossImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int indexNumber;

};
