// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledEntity.h"
#include "ObjectPoolActor.generated.h"


UCLASS()
class VRARTEST_API AObjectPoolActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AObjectPoolActor();

	virtual void Tick(float DeltaTime) override;

	APooledEntity* getAvailableEntity();

	void returnToPool(APooledEntity* entity);

	void turnOnEntity(APooledEntity* entity);

	void initialisePool(TSubclassOf<APooledEntity> poolTypeClass);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	
	const int poolSize = 10;
	
	UPROPERTY(EditAnywhere, Category = "Object Pool")
	TSubclassOf<APooledEntity> PooledEntityClass;
	
	TArray<APooledEntity*> EntityPool;
	
	APooledEntity* firstAvailable;

};
