// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "VRSpecific/VRCharacter.h"
#include "PowerUpBarWidget.generated.h"


UCLASS()
class VRARTEST_API UPowerUpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	// CreatePowerUpBar uses maps to map different EPowerUpType's to the different images associated with them, from there 5 blank images are created to
	// fill up the HorizontalBox that represents the powerup bar.
	void CreatePowerUpBar();

	// The addPowerUp function takes a EPowerUpType in it's parameter and uses this to set the image of the powerup paired with the type in the map.
	// The function iterates through the horizontal box until it finds a slot with it's powerup type set to NONE at whivh point it sets the value to type and 
	// replaces the image.
	void addPowerUp(EPowerUpType type);

	//iterates through the slots until it find the slot containing the powerUpType matching the one in the parameter at which point it will reset the PowerUpType to NONE
	// and reset the image
	void removePowerUp(EPowerUpType type);

	TArray<EPowerUpType> powerUpSlots;

	TArray<UImage*> powerUpImages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* AttackUpImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* AttackDownImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* SpeedUpImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* SpeedDownImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* clearImage;

	TMap<EPowerUpType, UTexture2D*> textureTypeMap;

protected: 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* powerUpBar;

};
