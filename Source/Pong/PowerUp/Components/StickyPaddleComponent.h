// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "../../PongPaddlePawn.h"
#include "PowerUp/PowerUpComponent.h"
#include "StickyPaddleComponent.generated.h"

UCLASS()
class PONG_API UStickyPaddleComponent : public UPowerUpComponent
{
	GENERATED_BODY()
	
private:
	virtual bool Behaviour() override;

	virtual void PaddleHitNotification(APongPaddlePawn* NewlyTouchedPaddle) override;

	FTimerHandle CountdownTimerHandle;

	void ReleaseBallFromPaddle();

	virtual void StopReceived() override;
};
