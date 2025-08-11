// Observer.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VRARTest.h"
#include "Observer.generated.h"

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
	EMPTY UMETA(DisplayName = "Empty")
};

UCLASS(Abstract, Blueprintable)
class VRARTEST_API UObserver : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Observer")
	void OnNotify(EEvent Event, float Value = 0.f, bool b = false);

	virtual void OnNotify_Implementation(EEvent Event, float Value = 0.f, bool bFlag = false) {};
};

