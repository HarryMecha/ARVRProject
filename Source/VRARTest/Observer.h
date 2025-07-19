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
	TREASURE_BUTTON,
	TRAP_BUTTON
};


class Observer
{
public:
	virtual ~Observer() = default;
	virtual void onNotify(Event event) = 0;
};


