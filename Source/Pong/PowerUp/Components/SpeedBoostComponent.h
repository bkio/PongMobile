// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "PowerUp/PowerUpComponent.h"
#include "SpeedBoostComponent.generated.h"

UCLASS()
class PONG_API USpeedBoostComponent : public UPowerUpComponent
{
	GENERATED_BODY()

private:
	virtual bool Behaviour() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);

	//Behaviour - Original value of paddle speed
	float BeforeBoostSpeedFactorOfPaddle;

	//Behaviour - After paddle picks this power-up, this will be the final value of easing paddle speed
	float DesiredSpeedFactorOfPaddle;

	//Behaviour - Count for easing in tick
	bool StartCounting_ForSpeedUpEasing;

	//Behaviour - Count for immediate speed down back
	float StartCounting_ForSpeedDown;

public:
	USpeedBoostComponent(const class FObjectInitializer& PCIP);
};
