// Fill out your copyright notice in the Description page of Project Settings.


#include "VRRepresentative.h"

// Sets default values
AVRRepresentative::AVRRepresentative()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = capsuleCollider;

	vrLeftHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Hand Sphere"));
	vrLeftHand->SetupAttachment(capsuleCollider);

	vrRightHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Hand Sphere"));
	vrRightHand->SetupAttachment(capsuleCollider);

	vrHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DwarfHead"));
	vrHead->SetupAttachment(capsuleCollider);

	vrBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DwarfHeadless"));
	vrBody->SetupAttachment(capsuleCollider);
}

// Called when the game starts or when spawned
void AVRRepresentative::BeginPlay()
{
	Super::BeginPlay();

	vrBody->PlayAnimation(idleAnimation, true);
	
}

// Called every frame
void AVRRepresentative::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

