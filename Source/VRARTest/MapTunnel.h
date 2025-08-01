// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARVRGameManager.h"
#include "MapTunnel.generated.h"

UENUM(BlueprintType)
enum class EWallState : uint8
{
	Down,
	MovingUp,
	Up,
	MovingDown
};

UCLASS()
class VRARTEST_API AMapTunnel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapTunnel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* tunnelMapMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* tunnelWallFrontMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* tunnelWallFrontCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* tunnelWallBackMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* tunnelWallBackCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* tunnelFrontArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* tunnelFrontArrowCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* tunnelBackArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* tunnelBackArrowCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* normalArrowMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	UMaterial* transparentArrowMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog")
	class UParticleSystemComponent* fogSystemFront;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fog")
	class UParticleSystemComponent* fogSystemBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	class UBoxComponent* boxColliderInner;

	virtual void Tick(float DeltaTime) override;

	void raiseAllWalls();

	void resetAllWalls();

	void turnOnFog(bool toggle);

	void toggleArrows(bool toggle);

	void toggleArrowVisibility(bool toggle);

	AARVRGameManager* manager;

	void setManager(AARVRGameManager* gameManager)
	{
		manager = gameManager;
	}

	bool getTunnelVisited()
	{
		return tunnelVisited;
	}


	void setTunnelVisited(bool toggle)
	{
		tunnelVisited = toggle;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:	
	
	float wallMoveSpeed = 3.0f;

	EWallState frontWallState = EWallState::Down;
	EWallState backWallState = EWallState::Down;


	bool tunnelVisited = false;

	float wallUpTargetZ = 0.0f;
	float wallDownTargetZ = -250.0f;
};
