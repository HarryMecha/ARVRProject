// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "VRCharacter.h"
#include "PowerUpBarWidget.generated.h"

UCLASS()
class VRARTEST_API UPowerUpBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void CreatePowerUpBar();

	void addPowerUp(EPowerUpType type);

	void removePowerUp(EPowerUpType type);

	TArray<EPowerUpType> powerUpSlots;

	TArray<UImage*> powerUpImages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texture")
	UTexture2D* AttackUpImage;

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
