// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledEntityComponent.h"
#include "PooledEntityInterface.h"
#include "StaticPooledEntity.generated.h"

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
	
	void entityFinished();

	bool isTransparent = false;

};
