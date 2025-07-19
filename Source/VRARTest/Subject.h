#pragma once

#include "CoreMinimal.h"
#include "Observer.h"



class Subject
{
public:

	void addObserver(Observer* observe)
	{
		if (observe && !observers.Contains(observe))
		{
			observers.Add(observe);
		}
	}
	void removeObserver(Observer* observe)
	{
		observers.Remove(observe);
	}

	void notify(Event event, float value = 0, bool b = false) const
	{
		for (Observer* obs : observers)
		{
			if (obs)
			{
				obs->onNotify(event, value,b);
			}
		}
	}

private:
	TArray<Observer*> observers;

};