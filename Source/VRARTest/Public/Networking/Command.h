// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRARTest.h"
#include "ARVRGameManager.h"
#include "ARSpecific/ARCharacter.h"
#include "ObjectPool/LivingPooledEntity.h"
#include "ObjectPool/PooledEntityInterface.h"
#include "ObjectPool/PooledEntityComponent.h"
#include "Map/MapSection.h"

// Standard parent class for all commands
class VRARTEST_API Command
{
public:
    
    virtual ~Command() = default;

    EMessageType commandType; //The command type.
  
    uint32 sequenceCount; //The sequence number of the command for tracking.

    float timeLastSent; //The in-game time at which it was sent.
   
    virtual void execute(AARVRGameManager* manager) = 0; // command specific logic that needs to be executed.

    // serialise() allows serialisation of the values in the command into an array of type uint8, in order to be sent over the network. It makes use an FMemoryWriter
    // in order to serialise values, these values must be serialsied and deserialised in a specfic order.
    virtual TArray<uint8> serialise(AARVRGameManager* manager) = 0; 

    // deserialise() allows deserialisation of the uint8 arrays recieved by the network convertingn them into commands in order to be able to interpret and execute their function.
    // It makes use an FMemoryReader in order to deserialise values, these values must be serialsied and deserialised in a specfic order.
    virtual void deserialise(AARVRGameManager* manager, TArray<uint8> packet) = 0; 



};

// Funtionality for PlayerMovementCommand can be found in NetworkCommand.cpp
class PlayerMovementCommand : public Command
{
public:

    FVector characterPosition = FVector::ZeroVector;
    FVector cameraPosition = FVector::ZeroVector;
    FVector leftHandPosition = FVector::ZeroVector;
    FVector rightHandPosition = FVector::ZeroVector;
    FVector entityPosition = FVector::ZeroVector;

    FRotator characterRotation = FRotator::ZeroRotator;
    FRotator cameraRotation = FRotator::ZeroRotator;
    FRotator leftHandRotation = FRotator::ZeroRotator;
    FRotator rightHandRotation = FRotator::ZeroRotator;
    FRotator entityRotation = FRotator::ZeroRotator;

    bool characterPositionChange = false;
    bool characterRotationChange = false;
    bool cameraPositionChange = false;
    bool cameraRotationChange = false;
    bool leftHandPositionChange = false;
    bool leftHandRotationChange = false;
    bool leftHandClosed = false;
    bool leftHandHammer = false;
    bool rightHandPositionChange = false;
    bool rightHandRotationChange = false;
    bool rightHandClosed = false;
    bool rightHandHammer = false;
    bool entityIncluded = false;

    virtual void execute(AARVRGameManager* manager) override;

    TArray<uint8> serialise(AARVRGameManager* manager) override;

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override;

};

// This is the command type for Wizard Movement it contains the characterPosition and characterRotation of the ARCharacter class, this is sent when the 
// ARCharacter moves in the AR Build of the game, in order to synchronise movement, the arCharacter instance within the VR players game will have their position and 
// rotation set to match, the booleans determine whether there was a value change on the AR Players game.
class WizardMovementCommand : public Command
{
public:

    FVector characterPosition = FVector::ZeroVector;

    FRotator characterRotation = FRotator::ZeroRotator;

    bool characterPositionChange = false;

    bool characterRotationChange = false;

    virtual void execute(AARVRGameManager* manager) override
    {
        AARCharacter* arCharacter = manager->getARCharacter();
        if (characterPositionChange)
        {
            arCharacter->SetActorRelativeLocation(characterPosition);
        }
        if (characterRotationChange)
        {
            arCharacter->SetActorRelativeRotation(characterRotation);
        }
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer << characterPositionChange;
        Writer << characterRotationChange;

        if (characterPositionChange) Writer << characterPosition;
        if (characterRotationChange) Writer << characterRotation;


        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(uint32));

        Reader << characterPositionChange;
        Reader << characterRotationChange;

        if (characterPositionChange) Reader << characterPosition;
        if (characterRotationChange) Reader << characterRotation;
    }

};

//This is the command type for arPlayerSelection it contains a boolean that simply determines whether the ar player has selected the plane that the map will be spawned on this allows for
// the VR player to begin sending positional updates.
class arPlayerSelectionCommand : public Command
{
public:

    virtual void execute(AARVRGameManager* manager) override
    {
        manager->arPlayerPlaneSelected = true;
        manager->clearIncomingQueue();
        manager->clearOutgoingQueue();
    }


    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(uint32));
    }

};

// The SpawnAtSectionCommand will be sent when the AR player spawns an entity at a certain section, execute will call the spawnEntityAtSection() function in the ARVRManager class, 
// this will spawn an object at the section with the index specified,  these indices are shared between both platforms.
class SpawnAtSectionCommand : public Command
{
public:

    uint32 sectionIndex;
    ESpawnableObject objectType;

    virtual void execute(AARVRGameManager* manager) override
    {
        manager->spawnEntityAtSection(manager->getMapSections()[sectionIndex], objectType);
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&sectionIndex, sizeof(sectionIndex));

        uint8 ObjectType = static_cast<uint8>(objectType);
        Writer.Serialize(&ObjectType, sizeof(uint8));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
            FMemoryReader Reader(packet, true);

            Reader.Serialize(&commandType, sizeof(uint8));

            Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

            Reader.Serialize(&sectionIndex, sizeof(sectionIndex));

            Reader.Serialize(&objectType, sizeof(uint8));

    }
};

// The InteractionAtSectionCommand will be sent when the VR player'S interaction with an enemy concludes, execute will call the interactionConclusion() function in the ARVRManager class, 
// this will return the object in that section to the pool at the section with the index specified,  these indices are shared between both platforms.
class InteractionAtSectionCommand : public Command
{
public:

    uint32 sectionIndex;

    virtual void execute(AARVRGameManager* manager) override
    {
        AMapSection* section = manager->getMapSections()[sectionIndex];
        manager->interactionConclusion(section->currentEntity);
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&sectionIndex, sizeof(sectionIndex));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&sectionIndex, sizeof(sectionIndex));

    }
};

//The SwitchTurnsCommand is receieved when the VR player concludes with their interaction with an entity in a section or an AR Player places a spell or object on the map, the execute 
// function simply calls the switchTurns command in the ARVRManager to enable the other play to player based on which player is currently playing.
class SwitchTurnsCommand : public Command
{
public:

    EPlayerRole playerTurn;

    virtual void execute(AARVRGameManager* manager) override
    {
        manager->switchTurns(playerTurn);
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        uint8 PlayerTurn = static_cast<uint8>(playerTurn);
        Writer.Serialize(&PlayerTurn, sizeof(uint8));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&playerTurn, sizeof(uint8));

    }
};

// The UpdateHealthCommand is sent when either player or an enemy takes damage in order to sync both plarforms, it uses a boolean to determine whether it was
// player who's health was affected or an enemy, this will call the ARVRManager's updatePlayerHealth function that will redirect the amount of health to it's
// relevant actor.
class UpdateHealthCommand : public Command
{
public:

    float amount;

    bool playerEffected;

    virtual void execute(AARVRGameManager* manager) override
    {
        if (playerEffected) {
            manager->updatePlayerHealth(amount);
        }
        else
        {
            manager->updateEnemyHealth(amount);
        }
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&amount, sizeof(amount));

        Writer.Serialize(&playerEffected, sizeof(playerEffected));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&amount, sizeof(amount));

        Reader.Serialize(&playerEffected, sizeof(playerEffected));


    }
};

// The BlockTunnelCommand is recieved when the AR character uses a block tunnel spell, it carries an index number of the section and maptunnel, this
// index number is used to find the section in the MapSections array and the specific index number of tunnel that needs to be set to blocked. These indices
// of both MapSections and MapTunnels are synced between systems.
class BlockTunnelCommand : public Command
{
public:

    uint32 sectionIndex;

    uint32 tunnelIndex;

    virtual void execute(AARVRGameManager* manager) override
    {
        AMapSection* mapSection = manager->getMapSections()[sectionIndex];
        AMapTunnel* mapTunnel = mapSection->getConnectedTunnels()[tunnelIndex];
        mapTunnel->setTunnelBlocked();
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&sectionIndex, sizeof(sectionIndex));

        Writer.Serialize(&tunnelIndex, sizeof(tunnelIndex));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&sectionIndex, sizeof(sectionIndex));

        Reader.Serialize(&tunnelIndex, sizeof(tunnelIndex));


    }
};

// The ReceiptConfirmationCommand is simply a callback command to confirm that certain commands have been recieved successfully.
class ReceiptConfirmationCommand : public Command
{
public:

    float sequenceOfReceipt;

    virtual void execute(AARVRGameManager* manager) override
    {
       
    }



    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&sequenceOfReceipt, sizeof(sequenceOfReceipt));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&sequenceOfReceipt, sizeof(sequenceOfReceipt));

    }
};

// The SwapSectionCommand is recieved when the AR character uses a swap objects spell, it carries an index number of both of the sections who's objects need swapping,
// the index numbers are used to find the sections in the MapSections array and swap the positions of the entities and currentEntity assignments in these sections. The indices
// of MapSections is synced between systems.
class SwapSectionCommand : public Command
{
public:
    uint32 section1Index;

    uint32 section2Index;

    virtual void execute(AARVRGameManager* manager) override
    {
        AMapSection* swapSelectedMapSection1 = manager->getMapSections()[section1Index];
        AMapSection* swapSelectedMapSection2 = manager->getMapSections()[section2Index];
        AActor* actorToSwap1 = swapSelectedMapSection1->getCurrentEntity();
        AActor* actorToSwap2 = swapSelectedMapSection2->getCurrentEntity();
        if (actorToSwap1)
        {
            UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(actorToSwap1->GetComponentByClass(UPooledEntityComponent::StaticClass()));

            if (pooledComponent)
            {
                pooledComponent->setOwnerSection(swapSelectedMapSection2);
            }
            else
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find PooledComponent!"));

            }
        }

        if (actorToSwap2)
        {
            UPooledEntityComponent* pooledComponent = Cast<UPooledEntityComponent>(actorToSwap2->GetComponentByClass(UPooledEntityComponent::StaticClass()));

            if (pooledComponent)
            {
                pooledComponent->setOwnerSection(swapSelectedMapSection1);
            }
            else
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find PooledComponent!"));

            }
        }
        swapSelectedMapSection1->spawnActorAtPoint(actorToSwap2);
        swapSelectedMapSection1->setCurrentEntity(actorToSwap2);
        swapSelectedMapSection2->spawnActorAtPoint(actorToSwap1);
        swapSelectedMapSection2->setCurrentEntity(actorToSwap1);
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&section1Index, sizeof(section1Index));

        Writer.Serialize(&section2Index, sizeof(section2Index));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&section1Index, sizeof(section1Index));

        Reader.Serialize(&section2Index, sizeof(section2Index));

    }
};

// The ApplyFrenzyCommand is recieved when the AR character uses a frenzy spell on an entity, it carries an index number for the section containing the entity,
// the index number is used to find the section in the MapSections array and apply Frenzy to the LivingPooledEntity at that position. The indices
// of MapSections is synced between systems.
class ApplyFrenzyCommand : public Command
{
public:
   
    uint32 sectionIndex;

    virtual void execute(AARVRGameManager* manager) override
    {
        AMapSection* selectedMapSection = manager->getMapSections()[sectionIndex];
        AActor* currentEntity = selectedMapSection->getCurrentEntity();
        if (currentEntity && currentEntity->IsA(ALivingPooledEntity::StaticClass()))
        {
            ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(currentEntity);
            if (livingEntity && livingEntity->getIsFrenzied() == false)
            {
                livingEntity->toggleFrenzyVR(true);
            }
        }
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        Writer.Serialize(&sectionIndex, sizeof(sectionIndex));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&sectionIndex, sizeof(sectionIndex));

    }
};

//The FireProjectileCommand is received when the AR player is in control of the ARCharacter and presses the fire button to shoot a projectile, this projectile object type
// is then sent over the network, it is used in the execute funtion to spawn an entity of that type at the spawn point attached to the ARCharacter, the functionality of this
// in the ARVRManager's spawnNonPlacableEntity function.
class FireProjectileCommand : public Command
{
public:

    ESpawnableObject objectType;

    virtual void execute(AARVRGameManager* manager) override
    {
        manager->spawnNonPlacableEntity(objectType);
    }

    TArray<uint8> serialise(AARVRGameManager* manager) override
    {
        TArray<uint8> packet;
        FMemoryWriter Writer(packet, true);

        uint8 MessageType = static_cast<uint8>(commandType);
        Writer.Serialize(&MessageType, sizeof(uint8));

        Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

        uint8 ObjectType = static_cast<uint8>(objectType);
        Writer.Serialize(&ObjectType, sizeof(uint8));

        return packet;
    }

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override
    {
        FMemoryReader Reader(packet, true);

        Reader.Serialize(&commandType, sizeof(uint8));

        Reader.Serialize(&sequenceCount, sizeof(sequenceCount));

        Reader.Serialize(&objectType, sizeof(uint8));

    }
};