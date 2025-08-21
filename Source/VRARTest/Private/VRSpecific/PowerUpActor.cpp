// Fill out your copyright notice in the Description page of Project Settings.


#include "VRSpecific/PowerUpActor.h"
#include "ObjectPool/ChestPooledEntity.h"

// Sets default values
APowerUpActor::APowerUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	RootComponent = boxCollider;

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	mesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void APowerUpActor::BeginPlay()
{
	Super::BeginPlay();
	mesh->SetVisibility(false);
	TArray<USceneComponent*> meshChildren;  
	mesh->GetChildrenComponents(true, meshChildren);
	if (meshChildren.Num() > 0)
	{
		for (USceneComponent* child : meshChildren)
		{
			child->SetVisibility(false);
		}
	}
	boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (boxCollider)
	{
		boxCollider->OnComponentBeginOverlap.AddDynamic(this, &APowerUpActor::OnColliderOverlap);
	}
}


void APowerUpActor::OnColliderOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AVRCharacter::StaticClass()) && OtherActor != this)
	{
		mesh->SetVisibility(false);
		AVRCharacter* character = Cast<AVRCharacter>(OtherActor);
		character->addPowerUp(powerUpType);
		boxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
