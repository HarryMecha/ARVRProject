// Fill out your copyright notice in the Description page of Project Settings.


#include "MapTunnel.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "VRCharacter.h"


// Sets default values
AMapTunnel::AMapTunnel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	tunnelMapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tunnelMapMesh"));
	tunnelMapMesh->SetupAttachment(RootComponent);

	tunnelWallFrontMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tunnelWallFrontMesh"));
	tunnelWallFrontMesh->SetupAttachment(RootComponent);
	tunnelWallFrontCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("tunnelWallFrontCollider"));
	tunnelWallFrontCollider->SetupAttachment(RootComponent);

	tunnelWallBackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tunnelWallBackMesh"));
	tunnelWallBackMesh->SetupAttachment(RootComponent);
	tunnelWallBackCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("tunnelWallBackCollider"));
	tunnelWallBackCollider->SetupAttachment(RootComponent);

	fogSystemFront = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("fogSystemFront"));
	fogSystemFront->SetupAttachment(RootComponent);

	fogSystemBack = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("fogSystemBack"));
	fogSystemBack->SetupAttachment(RootComponent);


	boxColliderInner = CreateDefaultSubobject<UBoxComponent>(TEXT("boxColliderInner"));
	boxColliderInner->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMapTunnel::BeginPlay()
{
	Super::BeginPlay();
	boxColliderInner->OnComponentBeginOverlap.AddDynamic(this, &AMapTunnel::OverlapBegin);
	boxColliderInner->OnComponentEndOverlap.AddDynamic(this, &AMapTunnel::OverlapEnd);
	tunnelWallFrontCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	tunnelWallBackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	turnOnFog(false);
}

// Called every frame
void AMapTunnel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (frontWallState) {
	case(EWallState::MovingUp):
	{
		FVector currentFrontWallLocation = tunnelWallFrontMesh->GetRelativeLocation();
		if (currentFrontWallLocation.Z < wallUpTargetZ)
		{
			float newWallLocation = FMath::FInterpTo(currentFrontWallLocation.Z, wallUpTargetZ, DeltaTime, wallMoveSpeed);
			tunnelWallFrontMesh->SetRelativeLocation(FVector(currentFrontWallLocation.X, currentFrontWallLocation.Y, newWallLocation));
		}
		if (currentFrontWallLocation.Z >= wallUpTargetZ)
			frontWallState = EWallState::Up;
		break;
	}
	case(EWallState::MovingDown):
	{
		FVector currentFrontWallLocation = tunnelWallFrontMesh->GetRelativeLocation();
		if (currentFrontWallLocation.Z > wallDownTargetZ)
		{
			float newWallLocation = FMath::FInterpTo(currentFrontWallLocation.Z, wallDownTargetZ, DeltaTime, wallMoveSpeed);
			tunnelWallFrontMesh->SetRelativeLocation(FVector(currentFrontWallLocation.X, currentFrontWallLocation.Y, newWallLocation));

			if (newWallLocation <= wallDownTargetZ)
			{
				tunnelWallFrontMesh->SetRelativeLocation(FVector(currentFrontWallLocation.X, currentFrontWallLocation.Y, wallDownTargetZ));
				frontWallState = EWallState::Down;
				tunnelWallFrontCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		break;
	}
	}
	switch (backWallState) {
	case(EWallState::MovingUp):
	{
		FVector currentBackWallLocation = tunnelWallBackMesh->GetRelativeLocation();
		if (currentBackWallLocation.Z < wallUpTargetZ)
		{
			float newWallLocation = FMath::FInterpTo(currentBackWallLocation.Z, wallUpTargetZ, DeltaTime, wallMoveSpeed);
			tunnelWallBackMesh->SetRelativeLocation(FVector(currentBackWallLocation.X, currentBackWallLocation.Y, newWallLocation));
		}
		if (currentBackWallLocation.Z >= wallUpTargetZ)
			backWallState = EWallState::Up;
		break;
	}
	case(EWallState::MovingDown):
	{
		FVector currentBackWallLocation = tunnelWallBackMesh->GetRelativeLocation();
		if (currentBackWallLocation.Z > wallDownTargetZ)
		{
			float newWallLocation = FMath::FInterpTo(currentBackWallLocation.Z, wallDownTargetZ, DeltaTime, wallMoveSpeed);
			tunnelWallBackMesh->SetRelativeLocation(FVector(currentBackWallLocation.X, currentBackWallLocation.Y, newWallLocation));

			if (newWallLocation <= wallDownTargetZ)
			{
				tunnelWallBackMesh->SetRelativeLocation(FVector(currentBackWallLocation.X, currentBackWallLocation.Y, wallDownTargetZ));
				backWallState = EWallState::Down;
				tunnelWallBackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		break;
	}
	}

}

void AMapTunnel::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AVRCharacter::StaticClass()) || OtherActor->Tags.Contains("VRRep"))
	{
		float distanceFromFront = FVector::Dist(OtherActor->GetActorLocation(), fogSystemFront->GetComponentLocation());
		float distanceFromBack = FVector::Dist(OtherActor->GetActorLocation(), fogSystemBack->GetComponentLocation());
		 if (OverlappedComponent == boxColliderInner) {
			if (distanceFromFront < distanceFromBack)
			{
				fogSystemFront->SetActive(false);
				fogSystemBack->SetActive(false);
				tunnelWallFrontCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				frontWallState = EWallState::MovingUp;
				manager->setLastTunnelVisited(this);
				if (OtherActor->IsA(AVRCharacter::StaticClass())) {
					AVRCharacter* vrCharacter = Cast<AVRCharacter>(OtherActor);
					manager->handleNextSection(this, vrCharacter->speedPowerUpCheck());
				}
			}
			else
			{
				fogSystemFront->SetActive(false);
				fogSystemBack->SetActive(false);
				tunnelWallBackCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				backWallState = EWallState::MovingUp;
				manager->setLastTunnelVisited(this);
				if (OtherActor->IsA(AVRCharacter::StaticClass())) {
				AVRCharacter* vrCharacter = Cast<AVRCharacter>(OtherActor);
				manager->handleNextSection(this, vrCharacter->getSpeedPowerUp());
				}
			}
		}
	}
}

void AMapTunnel::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(AVRCharacter::StaticClass())|| OtherActor->Tags.Contains("VRRep"))
	{
		 if (OverlappedComponent == boxColliderInner) {
			 
				tunnelVisited = true;
		}
	}
}

void AMapTunnel::raiseAllWalls()
{
	if (frontWallState == EWallState::Down || frontWallState == EWallState::MovingDown) {
		frontWallState = EWallState::MovingUp;
		tunnelWallFrontCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (backWallState == EWallState::Down || backWallState == EWallState::MovingDown) {
		backWallState = EWallState::MovingUp;
		tunnelWallBackCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AMapTunnel::resetAllWalls()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Turn Changed")));

	if (frontWallState == EWallState::Up || frontWallState == EWallState::MovingUp) {
		frontWallState = EWallState::MovingDown;
		tunnelWallFrontCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (backWallState == EWallState::Up || backWallState == EWallState::MovingUp) {
		backWallState = EWallState::MovingDown;
		tunnelWallBackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
}

void AMapTunnel::turnOnFog(bool toggle)
{
	if (tunnelVisited == false) {
		fogSystemFront->SetActive(toggle);
		fogSystemBack->SetActive(toggle);
	}

}

