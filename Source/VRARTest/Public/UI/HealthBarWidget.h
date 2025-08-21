// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "UI/HeartWidget.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// CreateHealthUI spawns HeartWidget objects on the healthbar, the amount of hearts depending on their maxHealth, these actors are adde to the heartWidgets array.
	void CreateHealthBar(int32 maxHealth);

	// updateHearts modifies the current health by the amount given in the parameter. This is then visually changes aswell by changing the 
	// image of the heartWidgets that make up the health bar to reflect this new amount, doing from right to left subtracting from an instance of the currentHealthAmount
	// minusing 1 for every full heart and setting it to 0 if a decimal is encountered.
	void updateHearts(float currentHealth);

	//setHealthBarHeaderText() takes an FString as an input, converts it to a TEXT object and then sets the healthBarheader to that object.
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
