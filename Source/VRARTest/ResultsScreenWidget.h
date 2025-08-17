// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ResultsScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UResultsScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void setResultText(FString text)
	{
		if (resultText)
		{
			resultText->SetText(FText::FromString(text));
		}
	}

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* resultText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* resultsBackgroundImage;
};
