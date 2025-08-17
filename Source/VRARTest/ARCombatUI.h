// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "HealthBarWidget.h"
#include "DamageFlashWidget.h"
#include "Subject.h"
class UUIObserver;
class AARCharacter;
#include "ARCombatUI.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UARCombatUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;


	UHealthBarWidget* getDwarfHealthBar()
	{
		return dwarfHealthBar;
	}

	UHealthBarWidget* getWizardHealthBar()
	{
		return wizardHealthBar;
	}

	UFUNCTION(BlueprintCallable, Category = "UI")
	void setupUIObserver(AARCharacter* character);

	UDamageFlashWidget* getDamageFlashWidget()
	{
		return damageFlashWidget;
	}

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* fireButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* crosshairImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* dwarfHealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHealthBarWidget* wizardHealthBar;

	UPROPERTY()
	//subject for host button clicked
	USubject* buttonClicked = nullptr;

	UPROPERTY()
	UUIObserver* UIObserverInstance;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDamageFlashWidget* damageFlashWidget;


private:
	void OnButtonClicked(EEvent event);

	UFUNCTION()
	void OnFireButtonClicked()
	{
		OnButtonClicked(EEvent::FIRE_BUTTON);
	}
	float fireCooldown = 0.5f;

	float timeSinceLastFire = -1.0f;
};
