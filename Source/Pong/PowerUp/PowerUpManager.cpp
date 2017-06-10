// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "PowerUpManager.h"

#include "Components/SpeedBoostComponent.h"
#include "Components/StickyPaddleComponent.h"
#include "Components/InvisibleBallComponent.h"
#include "Components/ReverseKeysComponent.h"
#include "Components/ShrinkBallComponent.h"

APowerUpManager::APowerUpManager(const FObjectInitializer& ObjectInitializer) : AActor(ObjectInitializer)
{
	PowerUpManagerInstance = this;

	PrimaryActorTick.bCanEverTick = true;

	RootComponent = BillboardComponent = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(this, TEXT("BillboardComponent"));
	BillboardComponent->SetVisibility(false, false);
	PlayAreaBounds = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("PlayArea"));
	PlayAreaBounds->AttachTo(RootComponent);

	GameStarted = false;

	//Assigning initial values of counters
	//In order to be able to change or add new powerup types easily, we have defined this as TMap. 
	PowerUpCounters.Add(EPowerUpType::SpeedBoost, 0.0f);
	PowerUpCounters.Add(EPowerUpType::StickyPaddle, 0.0f);
	PowerUpCounters.Add(EPowerUpType::InvisibleBall, 0.0f);
	PowerUpCounters.Add(EPowerUpType::ReverseKeys, 0.0f);
	PowerUpCounters.Add(EPowerUpType::ShrinkBall, 0.0f);

	//Generate value arrays to be able to access with O(1) complexity in every tick
	//This should be written after defining what kind of powerups will be available which are defined as assignments above.
	PowerUpCounters.GenerateValueArray(Generated_ValueArray_From_PowerUpCounters);
	PowerUpCounters.GenerateKeyArray(Generated_KeyArray_From_PowerUpCounters);

	//How many different power-ups do we have
	NumberOfTypesOfPowerUps = PowerUpCounters.Num();
	for (int i = 0; i < NumberOfTypesOfPowerUps; i++)
	{
		CurrentFrequencies.Add(0.0f);
		PowerUpProperties.Add(FPowerUpProperty());
	}
}

//Just to make sure nothing has been changed in blueprint of this, as not intended.
void APowerUpManager::OnConstruction(const FTransform& Transform)
{
	//If a new element has been added to the array, we have to delete it immediately.
	if (PowerUpProperties.Num() > NumberOfTypesOfPowerUps)
	{
		PowerUpProperties.RemoveAt(NumberOfTypesOfPowerUps);
	}
	//If an element has been removed from the array, or all elements of the array have been removed, restore the array back
	else if (PowerUpProperties.Num() == 0 || PowerUpProperties.Num() < NumberOfTypesOfPowerUps)
	{
		PowerUpProperties.Empty();
		for (int i = 0; i < NumberOfTypesOfPowerUps; i++)
		{
			FPowerUpProperty CurrentFrequency = FPowerUpProperty();
			CurrentFrequency.PowerUpType = Generated_KeyArray_From_PowerUpCounters[i];
			PowerUpProperties.Add(CurrentFrequency);
		}
	}
	//If everything is proper, we have to update each element's PowerUpType of the array
	else
	{
		for (int i = 0; i < NumberOfTypesOfPowerUps; i++)
		{
			PowerUpProperties[i].PowerUpType = Generated_KeyArray_From_PowerUpCounters[i];
		}
	}

	AActor::OnConstruction(Transform);
}

//Definitions of properties that will be used to be able to access PowerUpManager instance directly.
APowerUpManager* APowerUpManager::PowerUpManagerInstance = nullptr;
APowerUpManager* const APowerUpManager::GetInstance()
{
	return PowerUpManagerInstance;
}

void APowerUpManager::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	if (GameStarted)
	{
		float CurrentCounter;
		for (int32 i = 0; i < NumberOfTypesOfPowerUps; i++)
		{
			CurrentCounter = Generated_ValueArray_From_PowerUpCounters[i] + DeltaTime;
			if (CurrentCounter >= CurrentFrequencies[i])
			{
				Generated_ValueArray_From_PowerUpCounters[i] = 0.0f; //Start recounting
				CurrentFrequencies[i] = FMath::FRandRange(PowerUpProperties[i].MinValueOfRandVal, PowerUpProperties[i].MaxValueOfRandVal);

				//At most 3 power ups can be spawned in the scene
				if (SpawnedPowerUps.Num() < 3)
				{
					if (!DoesThisKindOfPowerUpAlreadyExist(Generated_KeyArray_From_PowerUpCounters[i]))
					{
						SpawnPowerUp(Generated_KeyArray_From_PowerUpCounters[i], i);
					}
				}
			}
			else
			{
				Generated_ValueArray_From_PowerUpCounters[i] = CurrentCounter;
			}
		}
	}
}

void APowerUpManager::StartReceived(APongBall* BallInst, float PaddleZValue)
{
	PaddleZ = PaddleZValue;

	BallInstance = BallInst;

	//First assignment of time destinations for power-ups
	ResetCountersAndFrequencies();

	//And let the counting process begin for all power-ups
	GameStarted = true;
}
void APowerUpManager::StopReceived()
{
	//Stop the counting process for all power-ups
	GameStarted = false;

	//Destroy all spawned components
	for (int32 i = 0; i < SpawnedPowerUps.Num(); i++)
	{
		//Destroy collision component
		if (SpawnedPowerUps[i].CollisionComponent)
		{
			SpawnedPowerUps[i].CollisionComponent->DestroyComponent();
		}
		//Destroy PowerUpComponent
		if (SpawnedPowerUps[i].MeshAndBehaviourComponent)
		{
			SpawnedPowerUps[i].MeshAndBehaviourComponent->StopReceived();
			SpawnedPowerUps[i].MeshAndBehaviourComponent->DestroyComponent();
		}
	}
	SpawnedPowerUps.Empty();
	PaddleHitObservers.Empty();

	ResetCountersAndFrequencies();

	//Restore speed values of paddles back
	for (TActorIterator<APongPaddlePawn> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		ActorIterator->SpeedFactor = 1.0f;
	}
}

//Resets counters and frequencies to their default values
void APowerUpManager::ResetCountersAndFrequencies()
{
	for (int32 i = 0; i < NumberOfTypesOfPowerUps; i++)
	{
		CurrentFrequencies[i] = FMath::FRandRange(PowerUpProperties[i].MinValueOfRandVal, PowerUpProperties[i].MaxValueOfRandVal);
		Generated_ValueArray_From_PowerUpCounters[i] = 0.0f;
	}
}

void APowerUpManager::SpawnPowerUp(EPowerUpType PowerUpType, int32 ArrayIndex)
{
	if (LastInteractedPaddle == nullptr) return;

	FVector UncollidedLocation = GetUncollidedRandomLocationInPlayArea();
	if (UncollidedLocation.Equals(FVector::ZeroVector, 0.1f)) return;
	if (!BallInstance) return;

	UPowerUpComponent* CreatedComponent = nullptr;

	switch (PowerUpType)
	{
		case EPowerUpType::SpeedBoost:
		{
			CreatedComponent = NewObject<USpeedBoostComponent>(this);
			break;
		}
		case EPowerUpType::StickyPaddle:
		{
			CreatedComponent = NewObject<UStickyPaddleComponent>(this);
			break;
		}
		case EPowerUpType::InvisibleBall:
		{
			CreatedComponent = NewObject<UInvisibleBallComponent>(this);
			break;
		}
		case EPowerUpType::ReverseKeys:
		{
			CreatedComponent = NewObject<UReverseKeysComponent>(this);
			break;
		}
		case EPowerUpType::ShrinkBall:
		{
			CreatedComponent = NewObject<UShrinkBallComponent>(this);
			break;
		}
	}

	if (CreatedComponent)
	{
		//Setting sphere component up
		USphereComponent* NewSphereComponent = NewObject<USphereComponent>(this);
		NewSphereComponent->AttachTo(RootComponent);
		NewSphereComponent->RegisterComponent();
		NewSphereComponent->SetSphereRadius(BallInstance->GetSphereRadius() / 4); //Quarter size of the ball
		NewSphereComponent->SetCollisionProfileName(FName("OverlapAll"));
		NewSphereComponent->bGenerateOverlapEvents = true;
		NewSphereComponent->SetWorldLocation(UncollidedLocation);

		//Setting mesh and behaviour component up
		CreatedComponent->AttachTo(NewSphereComponent); //Attach to created sphere component
		CreatedComponent->RegisterComponent();
		CreatedComponent->SetMobility(EComponentMobility::Movable);
		CreatedComponent->SetCollisionProfileName(FName("NoCollision"));
		if (PowerUpStaticMesh)
		{
			CreatedComponent->SetStaticMesh(PowerUpStaticMesh); //Set Mesh
			CreatedComponent->SetMaterial(0, PowerUpProperties[ArrayIndex].PowerUpMaterial); //Set Material
		}
		CreatedComponent->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f)); //Quarter size as well
		CreatedComponent->SetPowerUpComponent(BallInstance);

		//Add collision observer that notifies behaviour component
		NewSphereComponent->OnComponentBeginOverlap.AddDynamic(CreatedComponent, &UPowerUpComponent::OnOverlap);

		//Adding created components to the list
		FPowerUpComponentsElement NewElement;
		NewElement.PowerUpType = PowerUpType;
		NewElement.MeshAndBehaviourComponent = CreatedComponent;
		NewElement.CollisionComponent = NewSphereComponent;
		SpawnedPowerUps.Add(NewElement);
	}
}

//Checks if there is a same kind of powerup is already spawned or not
bool APowerUpManager::DoesThisKindOfPowerUpAlreadyExist(EPowerUpType PowerUpType)
{
	bool DoesExist = false;

	for (int32 i = 0; i < SpawnedPowerUps.Num(); i++)
	{
		if (SpawnedPowerUps[i].PowerUpType == PowerUpType)
		{
			DoesExist = true;
			break;
		}
	}
	return DoesExist;
}

//Checks if there is a spawned powerup in this location or not
bool APowerUpManager::IsThereAnyOtherPowerUps(FVector location)
{
	bool Collided = false;

	float SphereRadius;
	FVector CurrentComponentLocation;
	for (int32 i = 0; i < SpawnedPowerUps.Num(); i++)
	{
		if (SpawnedPowerUps[i].MeshAndBehaviourComponent && SpawnedPowerUps[i].CollisionComponent) //Nullptr control
		{
			SphereRadius = SpawnedPowerUps[i].CollisionComponent->GetUnscaledSphereRadius() * 4.0f; //We have multiplied radius with 4 because we need enough space between powerups.
			CurrentComponentLocation = SpawnedPowerUps[i].MeshAndBehaviourComponent->GetComponentLocation();

			if (location.X <= (CurrentComponentLocation.X + SphereRadius) && location.X >= (CurrentComponentLocation.X - SphereRadius) &&
				location.Y <= (CurrentComponentLocation.Y + SphereRadius) && location.Y >= (CurrentComponentLocation.Y - SphereRadius))
			{
				Collided = true;
				break;
			}
		}
	}
	return Collided;
}

//Get a random location between paddles and in play area
FVector APowerUpManager::GetUncollidedRandomLocationInPlayArea()
{
	FVector LocationOfBox = PlayAreaBounds->GetComponentLocation();
	FVector BoxExtent = PlayAreaBounds->GetScaledBoxExtent();

	FVector result = FVector::ZeroVector;

	FVector tempVector;
	bool FoundUncollidedLocation = false;
	int32 LocalCounter = 0;

	while (!FoundUncollidedLocation && LocalCounter < 3) //If we have already tried 3 times and could not find a suitable place, that means scene is full of power ups.
	{
		tempVector = FVector(
			FMath::FRandRange(LocationOfBox.X - BoxExtent.X, LocationOfBox.X + BoxExtent.X),
			FMath::FRandRange(LocationOfBox.Y - BoxExtent.Y, LocationOfBox.Y + BoxExtent.Y),
			PaddleZ
		);

		if (!IsThereAnyOtherPowerUps(tempVector))
		{
			FoundUncollidedLocation = true;
			result = tempVector;
		}
		LocalCounter++;
	}
	return result;
	
}

//Destroys the spawned component which is defined as parameter and clears it's reference from relative array
void APowerUpManager::DestroySpawnedPowerUpComponent(UPowerUpComponent* ComponentToDestroy)
{
	if (ComponentToDestroy == nullptr) return;

	for (int32 i = 0; i < SpawnedPowerUps.Num(); i++)
	{
		//If we have found the component that is wanted to be destroyed
		if (SpawnedPowerUps[i].MeshAndBehaviourComponent == ComponentToDestroy)
		{
			//Destroy collision component
			if (SpawnedPowerUps[i].CollisionComponent) //We have to check if it's null or not, because it might be destroyed by MarkSpawnedPowerUpComponentAsInactive before
			{
				SpawnedPowerUps[i].CollisionComponent->DestroyComponent();
			}

			//Remove PowerUpComponent from PaddleHitObservers if be contained
			PaddleHitObservers.Remove(ComponentToDestroy);

			//Destroy PowerUpComponent
			ComponentToDestroy->DestroyComponent();

			SpawnedPowerUps.RemoveAt(i);
			break;
		}
	}
}
//Does not destroy the spawned component which is defined as parameter but sets it and it's spherecomponent as invisible and inactive
void APowerUpManager::MarkSpawnedPowerUpComponentAsInactive(UPowerUpComponent* ComponentToInactivate)
{
	if (ComponentToInactivate == nullptr) return;

	for (int32 i = 0; i < SpawnedPowerUps.Num(); i++)
	{
		//If we have found the component that is wanted to be destroyed
		if (SpawnedPowerUps[i].MeshAndBehaviourComponent == ComponentToInactivate)
		{
			if (SpawnedPowerUps[i].CollisionComponent)
			{
				SpawnedPowerUps[i].CollisionComponent->DestroyComponent();
				SpawnedPowerUps[i].CollisionComponent = nullptr;
			}
			//Hide Component
			ComponentToInactivate->SetVisibility(false, false);
			break;
		}
	}
}

//This will be executed when ball hits a paddle
void APowerUpManager::InteractedWithPaddle(APongPaddlePawn* Paddle)
{
	if (Paddle == nullptr) return;

	LastInteractedPaddle = Paddle;

	//Notify all PowerUpComponents that want to be notified
	UPowerUpComponent* CurrentComponent;
	for (int32 i = 0; i < PaddleHitObservers.Num(); i++)
	{
		CurrentComponent = PaddleHitObservers[i];
		if (CurrentComponent)
		{
			CurrentComponent->PaddleHitNotification(Paddle);
		}
	}
}

//Add observer to PaddleHit listeners
void APowerUpManager::AddPaddleHitObserver(UPowerUpComponent* Observer)
{
	if (Observer == nullptr) return;

	//Add component to listeners array
	PaddleHitObservers.AddUnique(Observer);
}