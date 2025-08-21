#pragma once

#include "CoreMinimal.h"
#include "Observer.h"
#include "EngineUtils.h"
#include "VRSpecific/VRCharacter.h"
class AARPawn;
class AARCharacter;
class UUIConnectionWidget;
class AUDPCommunicationsManager;

#include "UIObserver.generated.h"

UCLASS()
class VRARTEST_API UUIObserver : public UObserver
{
	GENERATED_BODY()

public:
	UUIObserver();

	UFUNCTION(BlueprintCallable, Category = "Observer")
	void init(UWorld* world, AARPawn* pawn, AARCharacter* character);

	// The OnNotify_Implementation takes in multiple values in it's constructor, most of these are not always necessary so default values have been added,
	// the Event value will be used in a switch case to determine which button logic should be performed. Look to UIObserver.cpp for explanations of each.
	void OnNotify_Implementation(EEvent event, float value = 0, bool b = false);

private:
	UPROPERTY()
	AUDPCommunicationsManager* udpManager;

	AARPawn* arPawn;
	
	AARCharacter* arCharacter;
};
