// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "PowerUp/PowerUpComponent.h"
#include "InvisibleBallComponent.generated.h"

UCLASS()
class PONG_API UInvisibleBallComponent : public UPowerUpComponent
{
	GENERATED_BODY()
	
private:
	virtual bool Behaviour() override;

	FTimerHandle CountdownTimerHandle;
	void SetBallVisibleAgain();

	float AIController_OldSpeedFactor;

	virtual void StopReceived() override;
};
