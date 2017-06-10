// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "PowerUpComponent.h"
#include "../PongBall.h"
#include "../PongPaddlePawn.h"

#include "GameFramework/Actor.h"
#include "PowerUpManager.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
	SpeedBoost 		UMETA(DisplayName = "Speed Boost"),
	StickyPaddle 	UMETA(DisplayName = "Sticky Paddle"),
	InvisibleBall 	UMETA(DisplayName = "Invisible Ball"),
	ReverseKeys		UMETA(DisplayName = "Reverse Keys"),
	ShrinkBall		UMETA(DisplayName = "Shrink Ball")
	/*
	* To add new PowerUp Types, 
	* 1: Add a new element to this enum
	* 2: Modify constructor's "Assigning initial values of counters" part
	* 3: Create a new component class extends UPowerUpComponent and define it's behaviour
	* 4: Modify SpawnPowerUp function
	* 5: Include newly created component's header file in PowerUpManager.cpp
	*/
};

USTRUCT()
struct FPowerUpProperty
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power Up")
		EPowerUpType PowerUpType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up")
		UMaterialInterface* PowerUpMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up")
		float MinValueOfRandVal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up")
		float MaxValueOfRandVal;

	FPowerUpProperty()
	{
		//Assigning default values
		MinValueOfRandVal = 5.0f;
		MaxValueOfRandVal = 10.0f;
		PowerUpMaterial = nullptr;
	}
};

USTRUCT()
struct FPowerUpComponentsElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		EPowerUpType PowerUpType;

	UPROPERTY()
		UPowerUpComponent* MeshAndBehaviourComponent;

	UPROPERTY()
		USphereComponent* CollisionComponent;
};

//Power-Up system is designed as Plug&Play.
//It is so easy to not using power-ups in game, just do not spawn any PowerUpManager instance in the scene.
UCLASS()
class PONG_API APowerUpManager : public AActor
{
	GENERATED_BODY()

private:
	//Counters for different types of power-up
	TMap<EPowerUpType, float> PowerUpCounters;
	TArray<float> Generated_ValueArray_From_PowerUpCounters;
	TArray<EPowerUpType> Generated_KeyArray_From_PowerUpCounters;

	//Current frequency that will cause spawning a type of power-up
	TArray<float> CurrentFrequencies;

	//How many different power-ups do we have, we use this to be able to access without exhausting data segment by calling Num() in every tick
	int32 NumberOfTypesOfPowerUps;

	//List that holds spawned power up components
	TArray<FPowerUpComponentsElement> SpawnedPowerUps;

	void SpawnPowerUp(EPowerUpType PowerUpType, int32 ArrayIndex);

	//Get a random location between paddles and in play area
	FVector GetUncollidedRandomLocationInPlayArea();

	//Checks if there is a same kind of powerup is already spawned or not
	bool DoesThisKindOfPowerUpAlreadyExist(EPowerUpType PowerUpType);

	//Checks if there is a spawned powerup in this location or not
	bool IsThereAnyOtherPowerUps(FVector location);

	//Resets counters and frequencies to their default values
	void ResetCountersAndFrequencies();

	//Paddle's z value in world coordinate
	float PaddleZ;

	//Is game started
	bool GameStarted;

	//Ball instance
	APongBall* BallInstance;

	//Applying observer pattern to notify PowerUpComponents properly
	TArray<UPowerUpComponent*> PaddleHitObservers;

	//There shall be only one instance of PowerUpManager, we can use a static APowerUpManager* variable to keep the instance.
	static APowerUpManager* PowerUpManagerInstance;
	
public:	
	APowerUpManager(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Billboard")
		UBillboardComponent* BillboardComponent;

	//Play area bounds
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power Up")
		UBoxComponent* PlayAreaBounds;

	//Range of PowerUp Frequencies, each of frequencies will be chosen randomly in specified range.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up")
		TArray<FPowerUpProperty> PowerUpProperties;

	//Static mesh of power up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power Up")
		UStaticMesh* PowerUpStaticMesh;

	//Last interacted paddle
	APongPaddlePawn* LastInteractedPaddle;

	//Destroys the spawned component which is defined as parameter and clears it's reference from relative array
	void DestroySpawnedPowerUpComponent(UPowerUpComponent* ComponentToDestroy);

	//Does not destroy the spawned component which is defined as parameter but sets it and it's spherecomponent as invisible and inactive
	void MarkSpawnedPowerUpComponentAsInactive(UPowerUpComponent* ComponentToInactivate);

	//It is important to expose enough property for data encapsulation, we should not expose instance itself directly.
	static APowerUpManager* const GetInstance();

	//This will be executed when player press on start button
	void StartReceived(APongBall* BallInst, float PaddleZValue);

	//This will be executed when score increases
	void StopReceived();

	//This will be executed when ball hits a paddle
	void InteractedWithPaddle(APongPaddlePawn* Paddle);

	//Add observer to PaddleHit listeners
	void AddPaddleHitObserver(UPowerUpComponent* Observer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Power Up")
	void PowerUpActivated(EPowerUpType PowerUpType, bool IsActivatedForPlayer);

	//Just to make sure nothing has been changed in blueprint of this, as not intended.
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;
};
