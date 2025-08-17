#pragma once

#include "CoreMinimal.h"
#include "Observer.h"
#include "EngineUtils.h"
#include "VRCharacter.h"
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

	void OnNotify_Implementation(EEvent event, float value = 0, bool b = false);

private:
	UPROPERTY()
	AUDPCommunicationsManager* udpManager;

	AARPawn* arPawn;
	
	AARCharacter* arCharacter;
};
