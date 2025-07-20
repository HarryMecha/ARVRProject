#pragma once

#include "CoreMinimal.h"
#include "Observer.h"
#include "Subject.generated.h"


UCLASS()
class VRARTEST_API USubject : public UObject
{

	GENERATED_BODY()

public:

	void addObserver(UObserver* observe)
	{
		if (observe) 
		{
			if (!observers.Contains(observe)) {
				observers.Add(observe);
			}
		}
	}
	void removeObserver(UObserver* observe)
	{
		observers.Remove(observe);
	}

	void notify(EEvent event, float value = 0, bool b = false) const
	{
		for (TWeakObjectPtr<UObserver> obsPtr : observers)
		{
			if (obsPtr.IsValid())
			{
				obsPtr->OnNotify(event, value, b);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Observer broke")));

			}
		}
	}

private:
	TArray<TWeakObjectPtr<UObserver>> observers;

};