// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "VRMapSectionWidget.h"
#include "VRMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRARTEST_API UVRMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* MapBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MapCanvasPanel;

	UVRMapSectionWidget* getMapSectionFromArray(int index);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* MarkerArrowImage;

	void changeMarkerVisibility(bool toggle);

	void updatePlayerMarker(int targetIndex);

	void updatePlayerMarkerRotation(FRotator playerRotation);

private:
	virtual void NativeConstruct() override;

	void sortMapSections();

	TArray<UVRMapSectionWidget*> mapSectionImages;
};
