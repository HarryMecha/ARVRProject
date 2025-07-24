// Fill out your copyright notice in the Description page of Project Settings.


#include "MapSection.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "VRCharacter.h" 
#include "ARVRGameManager.h"
#include "Command.h"



// Sets default values
AMapSection::AMapSection()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root scene component
    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // Create and attach mesh
    mapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    mapMesh->SetupAttachment(RootComponent);

    // Create and attach spawn point
    spawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("spawnPoint"));
    spawnPoint->SetupAttachment(RootComponent);

    boxColliderSection = CreateDefaultSubobject<UBoxComponent>(TEXT("boxColliderSection"));
    boxColliderSection->SetupAttachment(RootComponent);

    currentEntity = nullptr;

}

// Called when the game starts or when spawned
void AMapSection::BeginPlay()
{
    Super::BeginPlay();
    boxColliderSection->OnComponentBeginOverlap.AddDynamic(this, &AMapSection::OverlapBegin);
    TArray<AActor*> children;
    GetAllChildActors(children);
    if (children.Num() > 0)
    {
        for (AActor* child : children) {
            if (!mapMesh) {
                mapMesh = Cast<UStaticMeshComponent>(child->GetComponentByClass(UStaticMeshComponent::StaticClass()));
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Found Mesh: %s"), *child->GetName()));

            }
        }
    }
}

// Called every frame
void AMapSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapSection::swapSelectedMaterial() 
{
	if (mapMesh->GetMaterial(0) == regularMapMaterial) {
		isSelected = true;
		mapMesh->SetMaterial(0, selectedMapMaterial);
	}
	else if (mapMesh->GetMaterial(0) == selectedMapMaterial) {
		isSelected = false;
		mapMesh->SetMaterial(0, regularMapMaterial);
	}
}

void AMapSection::spawnActorAtPoint(AActor* actorToSpawn)
{
	actorToSpawn->SetActorLocation(spawnPoint->GetComponentLocation());

}

void AMapSection::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()) || OtherActor->Tags.Contains("VRRep"))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Character interacted")));
        if (arvrmanager->getCurrentlyOccupiedSection() != this) {
          toggleWalls(true);
          arvrmanager->setCurrentlyOccupiedSection(this);
        }
    }
}

void AMapSection::toggleFog(bool toggle) 
{
    for (AMapTunnel* tunnel : connectedTunnels)
    {
        tunnel->turnOnFog(toggle);
    }
}


void AMapSection::toggleWalls (bool toggle)
{
    for (AMapTunnel* tunnel : connectedTunnels)
    {
        if (toggle)
        {
            tunnel->raiseAllWalls();
        }
        else
        {
            tunnel->resetAllWalls();
        }
    }
}

void AMapSection::interactionConclusion(AActor* entity)
{
    arvrmanager->interactionConclusion(entity);
    TSharedPtr<InteractionAtSectionCommand> command = MakeShared<InteractionAtSectionCommand>();
    command->commandType = EMessageType::InteractionAtSection;

    command->sequenceCount = arvrmanager->getSequenceCount();

    command->sectionIndex = arvrmanager->getMapSections().Find(this);

    arvrmanager->AddToOutgoingCommandQueue(command);
}

