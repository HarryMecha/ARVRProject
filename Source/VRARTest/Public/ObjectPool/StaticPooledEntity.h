// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectPool/PooledEntityComponent.h"
#include "ObjectPool/PooledEntityInterface.h"
#include "StaticPooledEntity.generated.h"

// This is the base class for all the static entities such as Traps, Spells and Chests there specific functionalities are better seen in their individual
// header files.

UCLASS()
class VRARTEST_API AStaticPooledEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStaticPooledEntity();

	virtual void Tick(float DeltaTime) override;


	IPooledEntityInterface* getPoolInterface() const
	{
		return Cast<IPooledEntityInterface>(poolComponent);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* normalMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* transparentMaterial;

	virtual void toggleTransparent(bool toggle) PURE_VIRTUAL(AStaticPooledEntity::toggleTransparent, );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPooledEntityComponent* poolComponent;
	
	//This entityFinished() function calls the entityFinished function in it's owning mapSection attempting to return it to the pool.
	void entityFinished();

	bool isTransparent = false;

};
