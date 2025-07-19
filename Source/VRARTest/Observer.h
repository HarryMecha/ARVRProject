// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRARTest.h"
/**
 * 
 */
enum Event {
	HOST_BUTTON,
	CLIENT_BUTTON,
	CONFIRM_BUTTON,
	SLIDER_CHANGE,
	CONFIRM_BUTTON_MAIN,
	TREASURE_BUTTON,
	TRAP_BUTTON,
	LEFT_ROTATION,
	RIGHT_ROTATION,
	EMPTY
};


class Observer
{
public:
	virtual ~Observer() = default;
	virtual void onNotify(Event event, float value = 0, bool b = false) = 0;
};


