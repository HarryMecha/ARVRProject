// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PooledEntityInterface.h"
#include "PooledEntityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRARTEST_API UPooledEntityComponent : public UActorComponent, public IPooledEntityInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPooledEntityComponent();

	virtual void setInUse(bool isInUse) override;
	virtual bool getInUse() const override;
	virtual void setNext(UPooledEntityComponent* entity) override;
	virtual UPooledEntityComponent* getNext() const override;
	virtual void setOwnerSection(class AMapSection* section) override;
	virtual AMapSection* getOwnerSection() const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	bool inUse = false;
	UPooledEntityComponent* nextEntity = nullptr;
	AMapSection* ownerSection = nullptr;
};
