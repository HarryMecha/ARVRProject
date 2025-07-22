// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PooledEntityInterface.generated.h"

class UPooledEntityComponent; 
class AMapSection;

UINTERFACE(MinimalAPI)
class UPooledEntityInterface : public UInterface
{
	GENERATED_BODY()
};

class VRARTEST_API IPooledEntityInterface
{
	GENERATED_BODY()

public:
	// These must be implemented in classes that use this interface
	virtual void setInUse(bool InUse) = 0;
	virtual bool getInUse() const = 0;

	virtual void setNext(UPooledEntityComponent* nextEntity) = 0;
	virtual  UPooledEntityComponent* getNext() const = 0;

	virtual void setOwnerSection(AMapSection* Section) = 0;
	virtual AMapSection* getOwnerSection() const = 0;
};
