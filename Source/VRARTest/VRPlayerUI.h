// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.h"
#include "PowerUpBarWidget.h"
#include "DamageFlashWidget.h"
#include "VRPlayerUI.generated.h"

/**
 * 
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

protected:
	


private:
	
};