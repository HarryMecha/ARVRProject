#include "Command.h"

void PlayerMovementCommand::execute(AARVRGameManager* manager)
{
	AActor* vrRepresentative = manager->getVRRep();
	UStaticMeshComponent* vrLeftHand = nullptr;
	UStaticMeshComponent* vrRightHand = nullptr;
	UStaticMeshComponent* vrHead = nullptr;


	TSet<UActorComponent*> components = vrRepresentative->GetComponents();
	for (UActorComponent* component : components)
	{
		if (component->ComponentTags.Contains("Head")) {
			vrHead = Cast<UStaticMeshComponent>(component);
		}

		if (component->ComponentTags.Contains("Left Hand"))
			vrLeftHand = Cast<UStaticMeshComponent>(component);

		if (component->ComponentTags.Contains("Right Hand"))
			vrRightHand = Cast<UStaticMeshComponent>(component);
	}

	if (characterPositionChange)
	{
		vrRepresentative->SetActorRelativeLocation(characterPosition);
	}

	if (characterRotationChange)
	{
		FRotator adjustedRotation = characterRotation;
		adjustedRotation.Yaw += -90.0f;
		vrRepresentative->SetActorRelativeRotation(adjustedRotation);
	}
	

	/*
	if (cameraPositionChange)
	{
		FVector fixedPosition = FVector(cameraPosition.Y, cameraPosition.X, cameraPosition.Z);
		vrHead->SetRelativeLocation(fixedPosition);
	
	}
	*/

	if (cameraRotationChange)
	{
		FRotator fixedRotation = FRotator(cameraRotation.Roll, cameraRotation.Yaw, -cameraRotation.Pitch);
		vrHead->SetRelativeRotation(fixedRotation);

	}

	if (leftHandPositionChange)
{
    // Use the same coordinate swap as the commented camera position
		FVector fixedPosition = FVector(-leftHandPosition.Y, leftHandPosition.X, leftHandPosition.Z);
		vrLeftHand->SetRelativeLocation(fixedPosition);
}

if (leftHandRotationChange)
{
    // Use the same rotation transformation as the working head rotation (no +180)
    FRotator fixedRotation = FRotator(-leftHandRotation.Roll, leftHandRotation.Yaw, -leftHandRotation.Pitch);
    vrLeftHand->SetRelativeRotation(fixedRotation);
}

// Apply Right Controller Transform
if (rightHandPositionChange)
{
    // Same coordinate swap
    FVector fixedPosition = FVector(-rightHandPosition.Y, rightHandPosition.X, rightHandPosition.Z);
    vrRightHand->SetRelativeLocation(fixedPosition);
}

if (rightHandRotationChange)
{
    // Same rotation transformation as head (no +180)
    FRotator fixedRotation = FRotator(-rightHandRotation.Roll, rightHandRotation.Yaw, -rightHandRotation.Pitch);
    vrRightHand->SetRelativeRotation(fixedRotation);
}
}

TArray<uint8> PlayerMovementCommand::serialise(AARVRGameManager* manager)
{
	TArray<uint8> packet;
	FMemoryWriter Writer(packet, true);

	uint8 MessageType = static_cast<uint8>(commandType);
	Writer.Serialize(&commandType, sizeof(uint8));

	Writer.Serialize(&sequenceCount, sizeof(sequenceCount));

	Writer << characterPositionChange;
	Writer << characterRotationChange;
	Writer << cameraPositionChange;
	Writer << cameraRotationChange;
	Writer << leftHandPositionChange;
	Writer << leftHandRotationChange;
	Writer << rightHandPositionChange;
	Writer << rightHandRotationChange;

	if (characterPositionChange) Writer << characterPosition;
	if (characterRotationChange) Writer << characterRotation;
	if (cameraPositionChange) Writer << cameraPosition;
	if (cameraRotationChange) Writer << cameraRotation;
	if (leftHandPositionChange)  Writer << leftHandPosition;
	if (leftHandRotationChange)  Writer << leftHandRotation;
	if (rightHandPositionChange) Writer << rightHandPosition;
	if (rightHandRotationChange) Writer << rightHandRotation;

	return packet;
}

void PlayerMovementCommand::deserialise(AARVRGameManager* manager, TArray<uint8> packet)
{
	FMemoryReader Reader(packet, true);

	Reader.Serialize(&commandType, sizeof(uint8));

	Reader.Serialize(&sequenceCount, sizeof(uint32));

	Reader << characterPositionChange;
	Reader << characterRotationChange;
	Reader << cameraPositionChange;
	Reader << cameraRotationChange;
	Reader << leftHandPositionChange;
	Reader << leftHandRotationChange;
	Reader << rightHandPositionChange;
	Reader << rightHandRotationChange;

	// Conditionally read values based on flags
	if (characterPositionChange) Reader << characterPosition;
	if (characterRotationChange) Reader << characterRotation;
	if (cameraPositionChange) Reader << cameraPosition;
	if (cameraRotationChange) Reader << cameraRotation;
	if (leftHandPositionChange)  Reader << leftHandPosition;
	if (leftHandRotationChange)  Reader << leftHandRotation;
	if (rightHandPositionChange) Reader << rightHandPosition;
	if (rightHandRotationChange) Reader << rightHandRotation;

}
