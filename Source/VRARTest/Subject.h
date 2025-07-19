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

	void notify(Event event) const
	{
		for (Observer* obs : observers)
		{
			if (obs)
			{
				obs->onNotify(event);
			}
		}
	}

private:
	TArray<Observer*> observers;

};