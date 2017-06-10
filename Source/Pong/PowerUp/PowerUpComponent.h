// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "../PongPaddlePawn.h"
#include "../PongBall.h"

#include "Components/StaticMeshComponent.h"
#include "PowerUpComponent.generated.h"

UCLASS()
class PONG_API UPowerUpComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	APongPaddlePawn* LastInteractedPaddle;
	APongBall* BallInstance;
	
public:
	//This returns if this component wants to be destroyed immediately or not. Even if this returns false, component will not be destroyed but be invisible.
	virtual bool Behaviour();

	//If this component is added to listener list, will get notification when ball hit a paddle 
	virtual void PaddleHitNotification(APongPaddlePawn* NewlyTouchedPaddle);

	//This will be executed when score increases
	virtual void StopReceived();

	void SetPowerUpComponent(APongBall* BallReference);

	UFUNCTION()
	void OnOverlap(class AActor * OtherActor, class UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
};
