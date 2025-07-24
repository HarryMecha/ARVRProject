// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "HeartWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void CreateHealthBar(int32 maxHealth);

	void updateHearts(float currentHealth);

	void setHealthBarHeaderText(FString text);

	TArray<UHeartWidget*> heartWidgets;


protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* healthBarheader;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* healthBar;

	UPROPERTY(EditAnywhere, Category = "Class Assignments")
	TSubclassOf<UHeartWidget> heartWidgetClass;

};
