#include "Command.h"
#include "VRRepresentative.h"
#include "LivingPooledEntity.h"

void PlayerMovementCommand::execute(AARVRGameManager* manager)
{
	AVRRepresentative* vrRepresentative = manager->getVRRep();
	UStaticMeshComponent* vrLeftHand = vrRepresentative->getVRLeftHand();
	UStaticMeshComponent* vrRightHand = vrRepresentative->getVRRightHand();
	UStaticMeshComponent* vrHead = vrRepresentative->getVRHead();
	USkeletalMeshComponent* vrBody = vrRepresentative->getVRBody();
	UAnimationAsset* idleAnimation = vrRepresentative->getIdleAnimation();
	UAnimationAsset* movingAnimation = vrRepresentative->getMovingAnimation();

	if (characterPositionChange)
	{
		vrRepresentative->SetActorRelativeLocation(characterPosition);
	
		if (!vrBody->IsPlaying() || vrBody->AnimationData.AnimToPlay != movingAnimation)
		{
			vrBody->PlayAnimation(movingAnimation, true);
		}

	}
	else 
	{
		if (!vrBody->IsPlaying() || vrBody->AnimationData.AnimToPlay != idleAnimation)
		{
			vrBody->PlayAnimation(idleAnimation, true);
		}
	}

	if (characterRotationChange)
	{
		FRotator adjustedRotation = characterRotation;
		adjustedRotation.Yaw += -90.0f;
		vrRepresentative->SetActorRelativeRotation(adjustedRotation);
	}
	
	if (cameraRotationChange)
	{
		FRotator fixedRotation = FRotator(cameraRotation.Roll, cameraRotation.Yaw, -cameraRotation.Pitch);
		vrHead->SetRelativeRotation(fixedRotation);

	}

	if (leftHandPositionChange)
	{
		FVector fixedPosition = FVector(-leftHandPosition.Y, leftHandPosition.X, leftHandPosition.Z);
		vrLeftHand->SetRelativeLocation(fixedPosition);
	}

	if (leftHandRotationChange)
	{
    FRotator fixedRotation = FRotator(-leftHandRotation.Roll, leftHandRotation.Yaw, -leftHandRotation.Pitch);
    vrLeftHand->SetRelativeRotation(fixedRotation);
	}

	if (rightHandPositionChange)
	{
		FVector fixedPosition = FVector(-rightHandPosition.Y, rightHandPosition.X, rightHandPosition.Z);
		vrRightHand->SetRelativeLocation(fixedPosition);
	}

	if (rightHandRotationChange)
	{
		FRotator fixedRotation = FRotator(-rightHandRotation.Roll, rightHandRotation.Yaw, -rightHandRotation.Pitch);
		vrRightHand->SetRelativeRotation(fixedRotation);
	}
	if (entityIncluded)
	{
		if (manager->getCurrentlyOccupiedSection()->getCurrentEntity()) {
			AActor* entity = manager->getCurrentlyOccupiedSection()->getCurrentEntity();

			if (entity->IsA(ALivingPooledEntity::StaticClass()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("It's a LivingEntity"));

				ALivingPooledEntity* livingEntity = Cast<ALivingPooledEntity>(entity);
				if (livingEntity)
				{
					livingEntity->setEntityPosition(entityPosition);
					livingEntity->setEntityRotation(entityRotation);
				}
			}

		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("No Entity at section"));

		}
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
	Writer << entityIncluded;

	if (characterPositionChange) Writer << characterPosition;
	if (characterRotationChange) Writer << characterRotation;
	if (cameraPositionChange) Writer << cameraPosition;
	if (cameraRotationChange) Writer << cameraRotation;
	if (leftHandPositionChange)  Writer << leftHandPosition;
	if (leftHandRotationChange)  Writer << leftHandRotation;
	if (rightHandPositionChange) Writer << rightHandPosition;
	if (rightHandRotationChange) Writer << rightHandRotation;
	if (entityIncluded) Writer << entityPosition;
	if (entityIncluded) Writer << entityRotation;

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
	Reader << entityIncluded;

	// Conditionally read values based on flags
	if (characterPositionChange) Reader << characterPosition;
	if (characterRotationChange) Reader << characterRotation;
	if (cameraPositionChange) Reader << cameraPosition;
	if (cameraRotationChange) Reader << cameraRotation;
	if (leftHandPositionChange)  Reader << leftHandPosition;
	if (leftHandRotationChange)  Reader << leftHandRotation;
	if (rightHandPositionChange) Reader << rightHandPosition;
	if (rightHandRotationChange) Reader << rightHandRotation;
	if (entityIncluded) Reader << entityPosition;
	if (entityIncluded) Reader << entityRotation;

}
