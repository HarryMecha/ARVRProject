// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapSection.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "VRSpecific/VRCharacter.h" 
#include "ARVRGameManager.h"
#include "Networking/Command.h"
#include "Map/TorchActor.h"
#include "UI/VRMapWidget.h"



// Sets default values
AMapSection::AMapSection()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create root scene component
    
    USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;
    RootComponent->SetMobility(EComponentMobility::Movable);

    // Create and attach mesh
    mapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    mapMesh->SetupAttachment(RootComponent);

    arrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMeshComponent"));
    arrowMesh->SetupAttachment(RootComponent);
    arrowMesh->SetVisibility(false);

    // Create and attach spawn point
    spawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("spawnPoint"));
    spawnPoint->SetupAttachment(RootComponent);

    boxColliderSection = CreateDefaultSubobject<UBoxComponent>(TEXT("boxColliderSection"));
    boxColliderSection->SetupAttachment(RootComponent);

    torchHolder = CreateDefaultSubobject<USceneComponent>(TEXT("TorchHolder"));
    torchHolder->SetupAttachment(RootComponent);

    currentEntity = nullptr;

}

// Called when the game starts or when spawned
void AMapSection::BeginPlay()
{
    Super::BeginPlay();
    boxColliderSection->OnComponentBeginOverlap.AddDynamic(this, &AMapSection::OverlapBegin);
    TArray<AActor*> childActors;
    GetAllChildActors(childActors);
    if (childActors.Num() > 0)
    {
        for (AActor* child : childActors) {
            if (!mapMesh) {
                mapMesh = Cast<UStaticMeshComponent>(child->GetComponentByClass(UStaticMeshComponent::StaticClass()));
               //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Found Mesh: %s"), *child->GetName()));

            }
        }
    }

    childActors.Empty();
    GetAllChildActors(childActors);

    for (AActor* child : childActors)
    {
        ATorchActor* torch = Cast<ATorchActor>(child);
        if (IsValid(torch))
        {
            torchArray.Add(torch);
        }
    }

    for (int32 i = 0; i < torchArray.Num(); i++)
    {
        ATorchActor* torch = torchArray[i];
        if (!IsValid(torch))
        {
            torchArray.Remove(torch);
        }
    }

}


// Called every frame
void AMapSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapSection::swapSelectedMaterial(UMaterialInterface* materialToSwap)
{
	if (materialToSwap == selectedMapMaterial) {
		isSelected = true;
		mapMesh->SetMaterial(0, materialToSwap);
	}
	else
    {
		isSelected = false;
		mapMesh->SetMaterial(0, materialToSwap);
	}
}

void AMapSection::spawnActorAtPoint(AActor* actorToSpawn)
{
	actorToSpawn->SetActorLocation(spawnPoint->GetComponentLocation());

}

void AMapSection::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor->IsA(AVRCharacter::StaticClass()))
    {
        AVRCharacter* vrCharacter = Cast<AVRCharacter>(OtherActor);
        if (vrCharacter->getSpeedPowerUp() == false) {
            if (arvrmanager->getCurrentlyOccupiedSection() != this) {
                if (vrCharacter->GetRootComponent())
                {
                    toggleWalls(true);
                }
                if (!currentEntity) //Map Section is empty
                {
                    if (!finalSection) {
                        TSharedPtr<SwitchTurnsCommand> command = MakeShared<SwitchTurnsCommand>();
                        command->commandType = EMessageType::SwitchTurns;

                        command->sequenceCount = arvrmanager->getNextSequenceCount();

                        command->playerTurn = EPlayerRole::AR;

                        arvrmanager->AddToOutgoingCommandQueue(command);
                        arvrmanager->switchTurns(EPlayerRole::AR);
                    }
                }
                int currentindex = arvrmanager->getMapSections().Find(arvrmanager->getCurrentlyOccupiedSection());
                if (currentindex != 0 && currentindex != 26)
                {
                    vrCharacter->vrMapWidget->getMapSectionFromArray(currentindex - 1)->changeCrossVisibility(true);

                }
                else
                {
                    vrCharacter->vrMapWidget->changeMarkerVisibility(false);
                }
                arvrmanager->setCurrentlyOccupiedSection(this);
                currentindex = arvrmanager->getMapSections().Find(arvrmanager->getCurrentlyOccupiedSection());
                if (currentindex != 0 && currentindex != 26)
                {
                    vrCharacter->vrMapWidget->updatePlayerMarker(currentindex - 1);
                }
                vrCharacter->vrMapWidget->changeMarkerVisibility(true);
                if (!sectionVisited)
                {
                    sectionVisited = true;
                    int index = arvrmanager->getMapSections().Find(this);
                    if (index != 0 && index != 26)
                    {
                        vrCharacter->vrMapWidget->getMapSectionFromArray(index - 1)->changeCrossVisibility(false);

                    }
                    else
                    {
                        vrCharacter->vrMapWidget->changeMarkerVisibility(false);
                    }
                        for (ATorchActor* torch : torchArray)
                        {
                            if (IsValid(torch))
                            {
                                torch->toggleLight();
                            }
                        }
                }
            }
        }
        else
        {
            arvrmanager->setCurrentlyOccupiedSection(this);
            toggleWalls(false); //weird edge-case when using the speedpower up this fixes issue
        }
    }
    if (OtherActor->Tags.Contains("VRRep") && OtherComp->IsA(UCapsuleComponent::StaticClass()))
    {
            if (arvrmanager->getCurrentlyOccupiedSection() != this) {
                toggleWalls(true);
                arvrmanager->setCurrentlyOccupiedSection(this);
                if (!sectionVisited)
                {
                    sectionVisited = true;
                    
                    if (finalSection == true)
                    {
                        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Starting Wizard Combat")));
                        boxColliderSection->Deactivate();
                        arvrmanager->startWizardCombat();

                    }
                }
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

void AMapSection::toggleArrows(bool toggle)
{
    for (AMapTunnel* tunnel : connectedTunnels)
    {
        tunnel->toggleArrows(toggle);
    }
}

void AMapSection::interactionConclusion()
{
    arvrmanager->interactionConclusion(currentEntity);
    setSectionUsed(false);
    TSharedPtr<InteractionAtSectionCommand> command = MakeShared<InteractionAtSectionCommand>();
    command->commandType = EMessageType::InteractionAtSection;

    command->sequenceCount = arvrmanager->getNextSequenceCount();

    command->sectionIndex = arvrmanager->getMapSections().Find(this);

    arvrmanager->AddToOutgoingCommandQueue(command);
}

