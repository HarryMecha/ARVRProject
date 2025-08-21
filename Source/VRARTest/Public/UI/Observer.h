// Observer.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VRARTest.h"
#include "Observer.generated.h"

//This defines all of the different button types that exist in the project, these all having specific cases in the OnNotift function that handle different
// logic.
UENUM(BlueprintType)
enum class EEvent : uint8
{
	HOST_BUTTON UMETA(DisplayName = "Host Button"),
	CLIENT_BUTTON UMETA(DisplayName = "Client Button"),
	CONFIRM_BUTTON UMETA(DisplayName = "Confirm Button"),
	SLIDER_CHANGE UMETA(DisplayName = "Slider Change"),
	CONFIRM_BUTTON_MAIN UMETA(DisplayName = "Confirm Button Main"),
	TREASURE_BUTTON UMETA(DisplayName = "Treasure Button"),
	TRAP_BUTTON UMETA(DisplayName = "Trap Button"),
	LEFT_ROTATION UMETA(DisplayName = "Left Rotation"),
	RIGHT_ROTATION UMETA(DisplayName = "Right Rotation"),
	RESET_BUTTON UMETA(DisplayName = "Reset Button"),
	GOBLIN_BUTTON UMETA(DisplayName = "Goblin Button"),
	BLOCK_BUTTON UMETA(DisplayName = "Block Button"),
	SWAP_BUTTON UMETA(DisplayName = "Swap Button"),
	FRENZY_BUTTON UMETA(DisplayName =  "Frenzy Button"),
	ZOOM_BUTTON UMETA(DisplayName = "Zoom Button"),
	FIRE_BUTTON UMETA(DisplayName = "Fire Button"),
	EMPTY UMETA(DisplayName = "Empty")
};

UCLASS(Abstract, Blueprintable)
class VRARTEST_API UObserver : public UObject
{
	GENERATED_BODY()

public:

	//This is a required class that redirercts to OnNotify_Implementation().
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Observer")
	void OnNotify(EEvent Event, float Value = 0.f, bool b = false);
	
	// The OnNotify_Implementation takes in multiple values in it's constructor, most of these are not always necessary so default values have been added,
	// the Event value will be used in a switch case to determine which button logic should be performed. Look to UIObserver.cpp for explanations of each.
	virtual void OnNotify_Implementation(EEvent Event, float Value = 0.f, bool bFlag = false) {};
};

