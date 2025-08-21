// Fill out your copyright notice in the Description page of Project Settings.


#include "ARSpecific/ARCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Networking/Command.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AARCharacter::AARCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(GetRootComponent());
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	CharacterMesh->SetOwnerNoSee(true);
	CharacterMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
	camera->bUsePawnControlRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	heartRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeartRoot"));
	heartRoot->SetupAttachment(RootComponent);

	spellSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	spellSpawnPoint->SetupAttachment(RootComponent);

	maxHealth = 5;
}

// Called when the game starts or when spawned
void AARCharacter::BeginPlay()
{
	Super::BeginPlay();	

	currentMaxHealth = maxHealth;
	currentHealth = currentMaxHealth;

	FVector CharacterPosition = GetRootComponent()->GetRelativeLocation();

	//SetActorRelativeLocation(FVector(CharacterPosition.X, CharacterPosition.Y, 20.0f));

	prevCharacterPosition = GetRootComponent()->GetRelativeLocation();
	prevCharacterRotation = GetRootComponent()->GetRelativeRotation();

	if (isARSession == false)
	{
		CreateHealthUI();
	}

	UWorld* currentWorld = GetWorld();
	playerController = UGameplayStatics::GetPlayerController(currentWorld, 0);

	if (arCombatWidgetBlueprintClass)
	{
		arCombatWidget = CreateWidget<UARCombatUI>(playerController, arCombatWidgetBlueprintClass);
		if (arCombatWidget)
		{
			arCombatWidget->setupUIObserver(this);
			arCombatWidget->AddToViewport();
			arCombatWidget->getWizardHealthBar()->CreateHealthBar(currentMaxHealth);
			arCombatWidget->SetVisibility(ESlateVisibility::Hidden);

			playerController->SetInputMode(FInputModeGameAndUI().SetWidgetToFocus(nullptr));
			playerController->bShowMouseCursor = false;

		}
	}

	if (resultsWidgetBlueprintClass)
	{
		resultstWidget = CreateWidget<UResultsScreenWidget>(playerController, resultsWidgetBlueprintClass);
		if (resultstWidget)
		{
			resultstWidget->AddToViewport();
			resultstWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

// Called every frame
void AARCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isARSession)
	{
		TimeSinceLastPacket += DeltaTime;
		if (TimeSinceLastPacket >= PacketInterval)
		{
			TimeSinceLastPacket = 0.0f;
			hasCharacterMoved();
		}
	}

}

// Called to bind functionality to input
void AARCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveLeftRight_AR", this, &AARCharacter::moveWizardLeftRight);
	PlayerInputComponent->BindAxis("MoveForwardBack_AR", this, &AARCharacter::moveWizardForwardBack);
	PlayerInputComponent->BindAxis("TurnLeftRight_AR", this, &AARCharacter::turnWizardLeftRight);
	PlayerInputComponent->BindAxis("TurnUpDown_AR", this, &AARCharacter::turnWizardUpDown);
}

void AARCharacter::moveWizardForwardBack(float strength)
{
	if (Controller && FMath::Abs(strength) > KINDA_SMALL_NUMBER)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		float movementSpeed = 0.75f;
		AddMovementInput(Direction, strength * movementSpeed);
	}
}

void AARCharacter::moveWizardLeftRight(float strength)
{
	if (Controller && FMath::Abs(strength) > KINDA_SMALL_NUMBER)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		float movementSpeed = 0.75f;
		AddMovementInput(Direction, strength * movementSpeed);
	}
}

void AARCharacter::turnWizardLeftRight(float strength)
{
	if (Controller && FMath::Abs(strength) > KINDA_SMALL_NUMBER)
	{
		AddControllerYawInput(strength);
	}
}

void AARCharacter::turnWizardUpDown(float strength)
{
	if (Controller && FMath::Abs(strength) > KINDA_SMALL_NUMBER)
	{
		AddControllerPitchInput(strength);
	}
}

void AARCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	playerController = Cast<APlayerController>(NewController);

	playerController->ActivateTouchInterface(currentTouchInterface);

	FVector CharacterPosition = GetRootComponent()->GetRelativeLocation();

}

void AARCharacter::hasCharacterMoved()
{
	FVector CharacterPosition = GetRootComponent()->GetRelativeLocation();
	FRotator CharacterRotation = GetRootComponent()->GetRelativeRotation();

	TSharedPtr<WizardMovementCommand> command = MakeShared<WizardMovementCommand>();

	command->commandType = EMessageType::WizardMovement;

	command->sequenceCount = manager->getNextSequenceCount();

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

	if (command->characterPositionChange || command->characterRotationChange)
	{
		if (manager)
		{
			manager->AddToOutgoingCommandQueue(command);
		}
	}

}

void AARCharacter::takeDamage(float amount)
{
	currentHealth -= amount;
	float currentHealthAmount = currentHealth;
	for (int i = heartActors.Num() - 1; i >= 0; i--)
	{
		if (currentHealthAmount >= 1.0f)
		{
			heartActors[i]->setHeartState(EHeartFillState::FULL);
			currentHealthAmount -= 1.0f;
		}
		else
		{
			if (currentHealthAmount == 0.25f)
			{
				heartActors[i]->setHeartState(EHeartFillState::QUARTER);
				currentHealthAmount = 0.0f;
			}
			else if (currentHealthAmount == 0.5f)
			{
				heartActors[i]->setHeartState(EHeartFillState::HALF);
				currentHealthAmount = 0.0f;
			}
			else if (currentHealthAmount == 0.75f)
			{
				heartActors[i]->setHeartState(EHeartFillState::THREEQUARTER);
				currentHealthAmount = 0.0f;
			}
			else
			{
				heartActors[i]->setHeartState(EHeartFillState::EMPTY);
			}
		}
	}


	if (currentHealth <= 0.0f)
	{
		Die();
	}
}

// Handle death
void AARCharacter::Die()
{
	DestroyHealthUI();
}

void AARCharacter::CreateHealthUI()
{

	float spacing = 20.0f;

	for (int i = 0; i < currentMaxHealth; i++)
	{
		float horizontalOffset = (i - (currentMaxHealth - 1) / 2.0f) * spacing;

		FVector spawnLocation = GetActorLocation() + FVector(0, horizontalOffset, heartRoot->GetRelativeLocation().Z);
		FRotator spawnRotation = FRotator(0.0f, 90.0f, 0.0f);

		AHeartActor* heart = GetWorld()->SpawnActor<AHeartActor>(heartActorClass, spawnLocation, spawnRotation);
		if (heart)
		{
			heart->AttachToComponent(heartRoot, FAttachmentTransformRules::KeepWorldTransform);
			heartActors.Add(heart);
			heart->setHeartState(EHeartFillState::FULL);
		}
	}
}

void AARCharacter::DestroyHealthUI()
{
	for (AHeartActor* Heart : heartActors)
	{
		if (Heart)
		{
			Heart->Destroy();
		}
	}
	heartActors.Empty();
}

UARCombatUI* AARCharacter::getARCombatWidget() const
{
	return arCombatWidget;
}

UResultsScreenWidget* AARCharacter::getResultsScreenWidget() const
{
	return resultstWidget;
}

void AARCharacter::spawnSpell()
{
	manager->spawnNonPlacableEntity(ESpawnableObject::Spell);
	TSharedPtr<FireProjectileCommand> command = MakeShared<FireProjectileCommand>();

	command->commandType = EMessageType::FireProjectile;

	command->sequenceCount = manager->getNextSequenceCount();

	command->objectType = ESpawnableObject::Spell;

	manager->AddToOutgoingCommandQueue(command);
}