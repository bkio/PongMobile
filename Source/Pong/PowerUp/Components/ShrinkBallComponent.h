// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "PowerUp/PowerUpComponent.h"
#include "ShrinkBallComponent.generated.h"

UCLASS()
class PONG_API UShrinkBallComponent : public UPowerUpComponent
{
	GENERATED_BODY()
	
private:
	virtual bool Behaviour() override;

	FTimerHandle CountdownTimerHandle;
	void ResetBallSize();

	virtual void StopReceived() override;
};
