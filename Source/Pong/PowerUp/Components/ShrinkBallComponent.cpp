// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "ShrinkBallComponent.h"
#include "../PowerUpManager.h"

bool UShrinkBallComponent::Behaviour()
{
	UPowerUpComponent::Behaviour();

	if (LastInteractedPaddle == nullptr) return true;

	APowerUpManager::GetInstance()->PowerUpActivated(EPowerUpType::ShrinkBall, LastInteractedPaddle->AutoPossessPlayer != EAutoReceiveInput::Disabled);

	//Shrink the ball
	BallInstance->ChangeBallSize(true);

	//Shrink the ball for 5 seconds
	APowerUpManager::GetInstance()->GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &UShrinkBallComponent::ResetBallSize, 5.0f, true);

	//Should not be destroyed now, but should be invisible.
	return false;
}

void UShrinkBallComponent::ResetBallSize()
{
	APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	//Restore size of the ball
	BallInstance->ChangeBallSize(false);

	//Process has been completed, destroy this.
	APowerUpManager::GetInstance()->DestroySpawnedPowerUpComponent(this);
}

void UShrinkBallComponent::StopReceived()
{
	if (APowerUpManager::GetInstance())
	{
		APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}

	//Restore size of the ball
	if (BallInstance)
	{
		BallInstance->ChangeBallSize(false);
	}
}