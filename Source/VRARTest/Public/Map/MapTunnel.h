// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARVRGameManager.h"
#include "MapTunnel.generated.h"

//The EWallState enumerator detremines the current state of the walls in the tunnel, whether they are up or down or currently transitioning between the two.
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

	//Set up for blueprint
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* arrowMesh;

	void toggleArrowMesh(bool toggle)
	{
		arrowMesh->SetVisibility(toggle);
	}

	/*
		The Tick function in the mapTunnel class handles the movement of the walls up and down vertically based on their EWallState, it will check the wall state for the front
		and back walls each frame, if the state changes to either MovingUp or MovingDown, it will begin to move the wall actors Z position towards a target value, a value determined
		by their position in the blueprint, this is done by interpolation between the current wall position and this tagret position, given a specific speed value. Once the wall reaches
		the value it will change states accordingly.
	*/
	virtual void Tick(float DeltaTime) override;

	// the raiseAllWalls() will change both wall's state to MovingUp if they are not up already, tigger the conditions in Tick() to move the walls up.
	void raiseAllWalls();

	// the resetAllWalls() will change both wall's state to MovingDown if they are not down already, tigger the conditions in Tick() to move the walls down.
	void resetAllWalls();

	// turnOnFog() toggles the visisbility of the emissions from the particle systems at either end of the tunnel.
	void turnOnFog(bool toggle);

	// toggleArrows() will turn off the collider and visibility of the arrow actors on the ends of the tunnel.
	void toggleArrows(bool toggle);

	// toggleArrowVisibility() changes the material attached to the arrow actors mesh, from fully visible to transparent.
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

	bool getTunnelBlocked()
	{
		return tunnelBlocked;
	}

	void setTunnelBlocked()
	{
		swapSelectedMaterial(blockedTunnelMaterial);
		tunnelBlocked = true;
		raiseAllWalls();
		turnOnFog(false);
		tunnelVisited = true;
	}
	
	// swapSelectedMaterial() swaps takes a mateiralInterface and swaps the currrent current material of the map tunnel with this one.
	void swapSelectedMaterial(UMaterialInterface* materialToSwap);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Tunnel")
	bool specialTunnel = false;

	bool getSpeciaTunnel()
	{
		return specialTunnel;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* selectedTunnelMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* regularTunnelMaterial;

	UPROPERTY(EditAnywhere, Category = "Material")
	UMaterialInterface* blockedTunnelMaterial;

	//The OverlapBegin() funtion triggers when an actor enters the boxCollider attached to the actor, it calculates the distance between the actor and the two fog systems
	// it will then raise the wall that is directly behind the player and turn the colldier attached to that wall on so the player cannot pass through it, it will also call
	// the handleNextSection() function in the ARVRManager class which will determine the map section attached to this tunnel the player is moving towards and will handle logic for it.
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//The OverlapEnds() funtion triggers when an actor leaves the boxCollider attached to the actor, this simply toggles tunnelVisited.
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:	
	
	float wallMoveSpeed = 3.0f;

	EWallState frontWallState = EWallState::Down;
	EWallState backWallState = EWallState::Down;


	bool tunnelVisited = false;

	bool isSelected = false;

	float wallUpTargetZ = 0.0f;
	float wallDownTargetZ = -250.0f;

	bool tunnelLocked = false;

	bool tunnelBlocked = false;

};
