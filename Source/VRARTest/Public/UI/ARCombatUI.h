// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/HealthBarWidget.h"
#include "UI/DamageFlashWidget.h"
#include "UI/Subject.h"
class UUIObserver;
class AARCharacter;
#include "ARCombatUI.generated.h"

/**
 * This is just the setup class so the AR Combat UI Widget is visible in the blueprint, it contains little functionality only getters, setters and declerations.
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

	// setupUIObserver() function sets up functionality for the UIObserver to observe this specific class for button clicks.
	// A new instance of the obsever is created with the currentWorld and reference to the owning ARCharacter being passed in, 
	// when this is successfully done the subject is set so that it can notify the observer when a button is clicked, with the 
	// buttonClicked->addObserver(UIObserverInstance); adding it.
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
	USubject* buttonClicked = nullptr;

	UPROPERTY()
	UUIObserver* UIObserverInstance;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDamageFlashWidget* damageFlashWidget;


private:
	// The OnButtonClicked() function starts the fireCoolDown timer by setting timeSinceLastFire it also notifies the subject to notify the observer  that the specific 
	// fire button has been pressed and to deal with the logic.
	void OnButtonClicked(EEvent event);

	UFUNCTION()
	void OnFireButtonClicked()
	{
		OnButtonClicked(EEvent::FIRE_BUTTON);
	}
	float fireCooldown = 0.25f;

	float timeSinceLastFire = -1.0f;
};
