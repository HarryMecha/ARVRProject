// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/HealthBarWidget.h"
#include "UI/PowerUpBarWidget.h"
#include "UI/DamageFlashWidget.h"
#include "VRPlayerUI.generated.h"

/**
 * This is just the setup class so the AR Combat UI Widget is visible in the blueprint, it contains little functionality only getters, setters and declerations.
 */
UCLASS()
class VRARTEST_API UVRPlayerUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* dwarfHealthBar;

	UHealthBarWidget* getDwarfHealthBar()
	{
		return dwarfHealthBar;
	}

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UPowerUpBarWidget* powerUpBar;

	UPowerUpBarWidget* getPowerUpBar()
	{
		return powerUpBar;
	}

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDamageFlashWidget* damageFlashWidget;

	UDamageFlashWidget* getDamageFlashWidget()
	{
		return damageFlashWidget;
	}

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDamageFlashWidget* restoreFlashWidget;

	UDamageFlashWidget* getRestoreFlashWidget()
	{
		return restoreFlashWidget;
	}

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* FrozenImage;

	void toggleFrozenImage(bool toggle)
	{
		if (toggle == true)
		{
			FrozenImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			FrozenImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	void setPopUpText(FString text);

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* popUpText;

private:
	
};