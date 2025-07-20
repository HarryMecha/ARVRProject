// Fill out your copyright notice in the Description page of Project Settings.
#include "VRCharacter.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "ARVRGameManager.h"
#include "Command.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
	UWorld* currentWorld = GetWorld();

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//Creates the VRCamera and assigns it's parent to VRRoot
	VRCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	VRCamera->SetupAttachment(GetRootComponent());

	//Creates new Controller Object for the Left Controller and sets it's parent to VR Root and it's tracking to the controller
	leftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	leftController->SetupAttachment(GetRootComponent());
	leftController->SetTrackingSource(EControllerHand::Left);

	leftWidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("LeftWidgetInteraction"));
	leftWidgetInteraction->SetupAttachment(leftController);
	leftWidgetInteraction->InteractionSource = EWidgetInteractionSource::World;

	leftHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftHandMesh"));
	leftHandMesh->SetupAttachment(leftController);

	//Creates new Controller Object for the Right Controller and sets it's parent to VR Root and it's tracking to the controller
	rightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	rightController->SetupAttachment(GetRootComponent());
	rightController->SetTrackingSource(EControllerHand::Right);

	rightWidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("RightWidgetInteraction"));
	rightWidgetInteraction->SetupAttachment(rightController);
	rightWidgetInteraction->InteractionSource = EWidgetInteractionSource::World;

	rightHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightHandMesh"));
	rightHandMesh->SetupAttachment(rightController);
	
	leftControllerRayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftControllerRayMesh"));
	leftControllerRayMesh->SetupAttachment(leftController);
	leftControllerRayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	leftControllerRayMesh->SetVisibility(false);

	rightControllerRayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightControllerRayMesh"));
	rightControllerRayMesh->SetupAttachment(rightController);
	rightControllerRayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	rightControllerRayMesh->SetVisibility(true);
	rightControllerSelected = true;

	manager = nullptr;
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	prevCharacterPosition = GetRootComponent()->GetComponentLocation();
	prevCharacterRotation = GetRootComponent()->GetComponentRotation();
	prevCameraPosition = VRCamera->GetComponentLocation();
	prevCameraRotation = VRCamera->GetComponentRotation();
	prevLeftHandPosition = leftController->GetAttachmentRoot()->GetRelativeLocation();
	prevLeftHandRotation = leftController->GetAttachmentRoot()->GetRelativeRotation();
	prevRightHandPosition = rightController->GetAttachmentRoot()->GetRelativeLocation();
	prevRightHandRotation = rightController->GetAttachmentRoot()->GetRelativeRotation();

	leftHandMesh->SetStaticMesh(leftControllerOpenMesh);
	rightHandMesh->SetStaticMesh(rightControllerOpenMesh);

	AARVRGameManager* Manager = Cast<AARVRGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AARVRGameManager::StaticClass()));
	if (!Manager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find Manager!"));
	}
	manager = Manager;
	
	TArray<AActor*> AllActors;
	AActor* mapRoot = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);
	for (AActor* actor : AllActors)
	{
		if (actor->Tags.Contains("MapRoot"))
		{
			mapRoot = actor;
		}
	}
	if (mapRoot)
	{
		GetRootComponent()->AttachToComponent(mapRoot->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find mapRoot!"));
	}

}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (leftControllerSelected) 
	{
		controllerLineTrace(leftController);
	}
	if (rightControllerSelected)
	{
		controllerLineTrace(rightController);
	}

	TimeSinceLastPacket += DeltaTime;
	if (TimeSinceLastPacket >= PacketInterval)
	{
		TimeSinceLastPacket = 0.0f;
		hasCharacterMoved();
	}


	UWorld* currentWorld = GetWorld();
	playerController = UGameplayStatics::GetPlayerController(currentWorld, 0);

}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForwardBack", this, &AVRCharacter::MoveForwardBack);
	PlayerInputComponent->BindAxis("MoveLeftRight", this, &AVRCharacter::MoveLeftRight);

	PlayerInputComponent->BindAxis("TurnLeftRight", this, &AVRCharacter::TurnLeftRight);

	PlayerInputComponent->BindAction("UIInteractLeftTrigger", IE_Pressed, this, &AVRCharacter::LeftTriggerPressed);
	PlayerInputComponent->BindAction("UIInteractLeftTrigger", IE_Released, this, &AVRCharacter::LeftTriggerReleased);
	
	PlayerInputComponent->BindAction("UIInteractRightTrigger", IE_Pressed, this, &AVRCharacter::RightTriggerPressed);
	PlayerInputComponent->BindAction("UIInteractRightTrigger", IE_Released, this, &AVRCharacter::RightTriggerReleased);


}

void AVRCharacter::MoveForwardBack(float strength)
{
	if (Controller && FMath::Abs(strength) > KINDA_SMALL_NUMBER)
	{
		// Get forward direction relative to the camera yaw
		FRotator yawRotation(0, VRCamera->GetComponentRotation().Yaw, 0);
		FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, strength);
	}
}

void AVRCharacter::MoveLeftRight(float strength)
{
	if (Controller && FMath::Abs(strength) > KINDA_SMALL_NUMBER)
	{
		FRotator yawRotation(0, VRCamera->GetComponentRotation().Yaw, 0);
		FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, strength);
	}
}

void AVRCharacter::TurnLeftRight(float value)
{
	if (Controller && FMath::Abs(value) > KINDA_SMALL_NUMBER)
	{
		float turnRate = 45.0f;
		float deltaTime = GetWorld()->GetDeltaSeconds();

		FRotator currentRotation = GetActorRotation();

		AddControllerYawInput(value * turnRate * deltaTime);

	}
}

void AVRCharacter::LeftTriggerPressed()
{
	
	leftHandMesh->SetStaticMesh(leftControllerClosedMesh);
	if (leftWidgetInteraction)
	{
		leftWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		if (rightControllerSelected) {
			rightControllerSelected = false;
			rightControllerRayMesh->SetVisibility(false);
			rightWidgetInteraction->SetActive(false);

		}
			leftControllerSelected = true;
			leftControllerRayMesh->SetVisibility(true);
			leftWidgetInteraction->SetActive(true);

	}

}

void AVRCharacter::LeftTriggerReleased()
{
	leftHandMesh->SetStaticMesh(leftControllerOpenMesh);
	if (leftWidgetInteraction)
	{
		leftWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

void AVRCharacter::RightTriggerPressed()
{
	
	rightHandMesh->SetStaticMesh(rightControllerClosedMesh);
	if (rightWidgetInteraction)
	{
		rightWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		if (leftControllerSelected) {
			leftControllerSelected = false;
			leftControllerRayMesh->SetVisibility(false);
			leftWidgetInteraction->SetActive(false);
		}
			rightControllerSelected = true;
			rightControllerRayMesh->SetVisibility(true);
			rightWidgetInteraction->SetActive(true);
	}
	
}

void AVRCharacter::RightTriggerReleased()
{
	rightHandMesh->SetStaticMesh(rightControllerOpenMesh);
	if (rightWidgetInteraction)
	{
		rightWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

void AVRCharacter::controllerLineTrace(UMotionControllerComponent* controller) 
{

	UStaticMeshComponent* currentRayMesh = nullptr;
	if (controller == leftController) {
		currentRayMesh = leftControllerRayMesh;
	}
	else if (controller == rightController) {
		currentRayMesh = rightControllerRayMesh;
	}

	if (!currentRayMesh) return;

	FVector rayStart = controller->GetComponentLocation();
	FVector rayDirection = controller->GetForwardVector();
	FVector rayEnd = rayStart + (rayDirection * controllerRayLength);

	FHitResult hitResult;
	FCollisionQueryParams parameters;
	parameters.AddIgnoredActor(this);
	bool hasHit = GetWorld()->LineTraceSingleByChannel(hitResult, rayStart, rayEnd, ECollisionChannel::ECC_Visibility, parameters, FCollisionResponseParams());

	FVector actualRayEnd = hasHit ? hitResult.Location : rayEnd;

	float actualDistance = FVector::Dist(rayStart, actualRayEnd);

	FVector midPoint = rayStart + ((actualRayEnd - rayStart) * 0.5f);
	currentRayMesh->SetWorldLocation(midPoint);

	FVector rayVector = actualRayEnd - rayStart;
	FRotator rayRotation = rayVector.Rotation();

	currentRayMesh->SetWorldRotation(rayRotation + FRotator(90.0f, 0.0f, 0.0f));

	currentRayMesh->SetWorldScale3D(FVector(0.01f, 0.01f, actualDistance * 0.01f));

}

void AVRCharacter::hasCharacterMoved() 
{
	FVector CharacterPosition = GetRootComponent()->GetRelativeLocation();
	FRotator CharacterRotation = GetRootComponent()->GetRelativeRotation();

	FVector CameraPosition = VRCamera->GetRelativeLocation();
	FRotator CameraRotation = VRCamera->GetRelativeRotation();

	FVector LeftHandPosition = leftController->GetRelativeLocation();
	FRotator LeftHandRotation = leftController->GetRelativeRotation();

	FVector RightHandPosition = rightController->GetRelativeLocation();
	FRotator RightHandRotation = rightController->GetRelativeRotation();

	
	TSharedPtr<PlayerMovementCommand> command = MakeShared<PlayerMovementCommand>();

	command->commandType = AARVRGameManager::EMessageType::PlayerMovement;

	command->sequenceCount = manager->getSequenceCount();

	const float PositionTolerance = 0.1f;
	const float RotationTolerance = 0.1f;

	if (!CharacterPosition.Equals(prevCharacterPosition, PositionTolerance))
	{
		command->characterPositionChange = true;
		command->characterPosition = CharacterPosition;
		prevCharacterPosition = CharacterPosition;
	}

	if (!CharacterRotation.Equals(prevCharacterRotation, RotationTolerance))
	{
		command->characterRotationChange = true;
		command->characterRotation = CharacterRotation;
		prevCharacterRotation = CharacterRotation;
	}

	if (!CameraPosition.Equals(prevCameraPosition, PositionTolerance))
	{
		command->cameraPositionChange = true;
		command->cameraPosition = CameraPosition;
		prevCameraPosition = CameraPosition;
	}

	if (!CameraRotation.Equals(prevCameraRotation, RotationTolerance))
	{
		command->cameraRotationChange = true;
		command->cameraRotation = CameraRotation;
		prevCameraRotation = CameraRotation;
	}

	if (!LeftHandPosition.Equals(prevLeftHandPosition, PositionTolerance))
	{
		command->leftHandPositionChange = true;
		command->leftHandPosition = LeftHandPosition;
		prevLeftHandPosition = LeftHandPosition;
	}

	if (!LeftHandRotation.Equals(prevLeftHandRotation, RotationTolerance))
	{
		command->leftHandRotationChange = true;
		command->leftHandRotation = LeftHandRotation;
		prevLeftHandRotation = LeftHandRotation;
	}

	if (!RightHandPosition.Equals(prevRightHandPosition, PositionTolerance))
	{
		command->rightHandPositionChange = true;
		command->rightHandPosition = RightHandPosition;
		prevRightHandPosition = RightHandPosition;
	}

	if (!RightHandRotation.Equals(prevRightHandRotation,RotationTolerance))
	{
		command->rightHandRotationChange = true;
		command->rightHandRotation = RightHandRotation;
		prevRightHandRotation = RightHandRotation;
	}

	if (command->characterPositionChange || command->characterRotationChange ||
		command->cameraPositionChange || command->cameraRotationChange||
		command->leftHandPositionChange || command->leftHandRotationChange ||
		command->rightHandPositionChange || command->rightHandRotationChange)
	{
		if (manager)
		{
			manager->AddToOutgoingCommandQueue(command);
		}
	}
}



