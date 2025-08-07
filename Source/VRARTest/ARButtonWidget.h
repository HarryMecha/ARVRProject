// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "ARButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UARButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UButton* buttonWrapper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* buttonIcon;

	UButton* getButton()
	{
		return buttonWrapper;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* buttonImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidgetOptional))
	UImage* numberHolder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UTextBlock* numberText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* buttonHeader;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* buttonDescription;

	void setNumberText(int value)
	{
		FText text = FText::AsNumber(value);

		numberText->SetText(text);
	}
};
