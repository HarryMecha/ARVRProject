// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "UI/VRMapSectionWidget.h"
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

	//changeMarkerVisibility() changes marker visibility based on the value of the boolean.
	void changeMarkerVisibility(bool toggle);

	//updatePlayerMarker() will take the current position of the slot the marker currently occupies and set the position of the marker to 
	// the center of the new slot
	void updatePlayerMarker(int targetIndex);

	//updatePlayerMarkerRotation() takes in a FRotator and applies it to the markerImage.
	void updatePlayerMarkerRotation(FRotator playerRotation);

private:
	virtual void NativeConstruct() override;

	//This sortMapSections() function is used to physically sort the mapSection images so they match up with the mapSections array within
	// ARVRGameManager, for ease of use, this is done by sortin their index numbers that were assigned in the blueprint matching them to the index
	// in the array
	void sortMapSections();

	TArray<UVRMapSectionWidget*> mapSectionImages;
};
