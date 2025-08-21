#include "Networking/Command.h"
#include "ARSpecific/VRRepresentative.h"
#include "ObjectPool/LivingPooledEntity.h"

/*
	This is the command type for VR Players Movement it contains values of the posiitons and roataions of the charater itself, of the left and right motion controllers 
	the position of the vr camera, booleans the determine the current state of each hand; open, closed or holding a hammer and if another entity is within the same section 
	as the VRPlayer, if they are it pools these two positional updates. Therefore the command may also contain the positional and rotational values of a livingentity aswell.
	This command is sent when the VRCharacter and Entity moves in the VR Build of the game, in order to synchronise movement, the vrRpresentative and entity instance instance 
	within the AR players game will have their position and rotation set to match, specifically for the VR Player have each of it's different components matching, 
	the booleans determine whether there was a value change on the VR Players game of any of these components, if they were the values will change.
*/
void PlayerMovementCommand::execute(AARVRGameManager* manager)
{
	AVRRepresentative* vrRepresentative = manager->getVRRep();
	UStaticMeshComponent* vrLeftHand = vrRepresentative->getVRLeftHand();
	UStaticMeshComponent* vrRightHand = vrRepresentative->getVRRightHand();
	UStaticMeshComponent* vrHead = vrRepresentative->getVRHead();
	USkeletalMeshComponent* vrBody = vrRepresentative->getVRBody();
	UAnimationAsset* idleAnimation = vrRepresentative->getIdleAnimation();
	UAnimationAsset* movingAnimation = vrRepresentative->getMovingAnimation();

	//Controls animation status
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

	if (leftHandClosed == true)
	{
		vrLeftHand->SetStaticMesh(vrRepresentative->vrLeftHandClosed);

	}
	else if (leftHandHammer == true)
	{
		vrLeftHand->SetStaticMesh(vrRepresentative->vrLeftHandHammer);
	}
	else if (leftHandHammer == false && leftHandClosed == false)
	{
		vrLeftHand->SetStaticMesh(vrRepresentative->vrLeftHandOpen);
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

	if (rightHandClosed == true)
	{
		vrRightHand->SetStaticMesh(vrRepresentative->vrRightHandClosed);
	
	}
	else if (rightHandHammer == true)
	{
		vrRightHand->SetStaticMesh(vrRepresentative->vrRightHandHammer);
	}
	else if (rightHandHammer == false && rightHandClosed == false)
	{
		vrRightHand->SetStaticMesh(vrRepresentative->vrRightHandOpen);
	}

	// If entity is within the same map section on time of command send
	if (entityIncluded)
	{
		if (manager->getCurrentlyOccupiedSection()->getCurrentEntity()) {
			AActor* entity = manager->getCurrentlyOccupiedSection()->getCurrentEntity();

			if (entity->IsA(ALivingPooledEntity::StaticClass()))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("It's a LivingEntity"));

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
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("No Entity at section"));

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
	Writer << leftHandClosed;
	Writer << leftHandHammer;
	Writer << rightHandPositionChange;
	Writer << rightHandRotationChange;
	Writer << rightHandClosed;
	Writer << rightHandHammer;
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
	Reader << leftHandClosed;
	Reader << leftHandHammer;
	Reader << rightHandPositionChange;
	Reader << rightHandRotationChange;
	Reader << rightHandClosed;
	Reader << rightHandHammer;
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
