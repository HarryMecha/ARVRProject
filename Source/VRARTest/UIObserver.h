#pragma once

#include "CoreMinimal.h"
#include "Observer.h"
#include "EngineUtils.h"
#include "ARPawn.h"
#include "VRCharacter.h"
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

	void onNotify(Event event);

private:
	UPROPERTY()
	AUDPCommunicationsManager* udpManager;

	AARPawn* arPawn;
};
