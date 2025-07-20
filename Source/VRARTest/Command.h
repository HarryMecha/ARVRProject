// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRARTest.h"
#include "ARVRGameManager.h"
#include "MapSection.h"

class VRARTEST_API Command
{
public:
    
    virtual ~Command() = default;

    EMessageType commandType;

    uint32 sequenceCount;
   
    virtual void execute(AARVRGameManager* manager) = 0;

    virtual TArray<uint8> serialise(AARVRGameManager* manager) = 0;

    virtual void deserialise(AARVRGameManager* manager, TArray<uint8> packet) = 0;



};

class PlayerMovementCommand : public Command
{
public:

    FVector characterPosition = FVector::ZeroVector;
    FVector cameraPosition = FVector::ZeroVector;
    FVector leftHandPosition = FVector::ZeroVector;
    FVector rightHandPosition = FVector::ZeroVector;

    FRotator characterRotation = FRotator::ZeroRotator;
    FRotator cameraRotation = FRotator::ZeroRotator;
    FRotator leftHandRotation = FRotator::ZeroRotator;
    FRotator rightHandRotation = FRotator::ZeroRotator;

    bool characterPositionChange = false;
    bool characterRotationChange = false;
    bool cameraPositionChange = false;
    bool cameraRotationChange = false;
    bool leftHandPositionChange = false;
    bool leftHandRotationChange = false;
    bool rightHandPositionChange = false;
    bool rightHandRotationChange = false;

    virtual void execute(AARVRGameManager* manager) override;

    TArray<uint8> serialise(AARVRGameManager* manager) override;

    void deserialise(AARVRGameManager* manager, TArray<uint8> packet) override;

};

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
