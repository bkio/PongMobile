// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "PowerUp/PowerUpComponent.h"
#include "ReverseKeysComponent.generated.h"

UCLASS()
class PONG_API UReverseKeysComponent : public UPowerUpComponent
{
	GENERATED_BODY()

private:
	virtual bool Behaviour() override;

	FTimerHandle CountdownTimerHandle;
	void RestoreControllerBack();

	virtual void StopReceived() override;
};
