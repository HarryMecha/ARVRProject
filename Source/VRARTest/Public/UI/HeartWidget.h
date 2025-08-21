// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UI/HeartActor.h"
#include "HeartWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UHeartWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Defining all the images for allocation in the blueprint

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* heartImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* fullHeartTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* threeQuarterHeartTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* halfHeartTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* quarterHeartTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* emptyHeartTexture;

	//This will set the image of the heartWidget based on the input EHeartFillState.
	UFUNCTION(BlueprintCallable)
	void setHeartState(EHeartFillState newState);

	EHeartFillState getCurrentState()
	{
		return currentState;
	}

	UImage* getHeartImage()
	{
		return heartImage;
	}

protected:
	virtual void NativeConstruct() override;

	EHeartFillState currentState;

};
