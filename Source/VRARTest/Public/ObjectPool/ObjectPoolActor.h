// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPool/PooledEntity.h"
#include "ObjectPoolActor.generated.h"


UCLASS()
class VRARTEST_API AObjectPoolActor : public AActor
{
	GENERATED_BODY()
	
public:	

	AObjectPoolActor();

	virtual void Tick(float DeltaTime) override;

	//The getAvailableEntity() function, retreives the firstAvailable entity from the pool, calls the function turnOnEntity(), enabling it's behavoiur and making it visible 
	// and sets firstAvaiable to the next available entity defined in the pooledObject component of the retreived entity.
	AActor* getAvailableEntity();

	// returnToPool takes an actor in it's parameter, resets it's owner section and sets it to not in use, then gets the the firstavailable entity and sets this entitys next available
	// entity to that. The behaviour and visibility of this entity is then turned off.
	void returnToPool(AActor* entity);

	// turnOnEntity will enable all the entities behaviour such as it's tick and collider as well as making it visible.
	void turnOnEntity(AActor* entity);

	/*
		initialisePool() is the function used to initialise the entity pool, it starts buy creating objects up the pool size defined by the class given in it's parameter. It attachs these 
		objects to the map root so they willbe conform to the scale and position on the AR build but also so relatvie positions and rotations can be used when they're positions are transmitted
		to the other player, resulting on synced movement. Once all the entities have been created, the array is re-iterated through setting the next available entity pointer of each of the entities
		to the next entity in the array, apart from the last entity who has this pointer set to null. The first available entity is then assigned to the first member of this array.  
	*/
	void initialisePool(TSubclassOf<AActor> poolTypeClass);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	
	const int poolSize = 10;
	
	UPROPERTY(EditAnywhere, Category = "Object Pool")
	TSubclassOf<AActor> PooledEntityClass;
	
	TArray<AActor*> EntityPool;
	
	AActor* firstAvailable;

};
