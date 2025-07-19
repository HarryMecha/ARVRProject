#pragma once

#include "CoreMinimal.h"
#include "Observer.h"
#include "EngineUtils.h"
#include "VRCharacter.h"
class AARPawn;
class UUIConnectionWidget;
class AUDPCommunicationsManager;

#include "UIObserver.generated.h"

UCLASS()
class VRARTEST_API UUIObserver : public UObject, public Observer
{
	GENERATED_BODY()

public:
	UUIObserver();

	UFUNCTION(BlueprintCallable, Category = "Observer")
	void init(UWorld* world, AARPawn* pawn);

	void onNotify(Event event, float value = 0, bool b = false);

private:
	UPROPERTY()
	AUDPCommunicationsManager* udpManager;

	AARPawn* arPawn;
};
