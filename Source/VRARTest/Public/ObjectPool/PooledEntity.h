// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PooledEntity.generated.h"

UCLASS()
class VRARTEST_API APooledEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APooledEntity() = default;

	bool inUse = false;

	APooledEntity* nextEntity = nullptr;

	bool getInUse() const { return inUse; }

	void setInUse(bool isInUse) { inUse = isInUse; }

	APooledEntity* getNext() const { return nextEntity; }

	void setNext(APooledEntity* entity) { nextEntity = entity; }

	class AMapSection* ownerSection;

	void setOwnerSection(AMapSection* section) 
	{
		ownerSection = section;
	}

	AMapSection* getOwnerSection() 
	{
		return ownerSection;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* entityMesh = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime);

};
