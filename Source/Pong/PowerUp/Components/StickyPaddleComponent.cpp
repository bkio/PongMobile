// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "StickyPaddleComponent.h"
#include "../PowerUpManager.h"

bool UStickyPaddleComponent::Behaviour()
{
	UPowerUpComponent::Behaviour();

	if (LastInteractedPaddle == nullptr) return true;

	APowerUpManager::GetInstance()->PowerUpActivated(EPowerUpType::StickyPaddle, LastInteractedPaddle->AutoPossessPlayer != EAutoReceiveInput::Disabled);

	if(APowerUpManager::GetInstance())
	{
		APowerUpManager::GetInstance()->AddPaddleHitObserver(this);
	}

	//Should not be destroyed now, but should be invisible.
	return false;
}

void UStickyPaddleComponent::PaddleHitNotification(APongPaddlePawn* NewlyTouchedPaddle)
{
	UPowerUpComponent::PaddleHitNotification(NewlyTouchedPaddle);

	//If ball hit by owner paddle again 
	if (LastInteractedPaddle == NewlyTouchedPaddle)
	{
		BallInstance->IsCurrentlySticked = true;
		BallInstance->StickedTo = NewlyTouchedPaddle;

		APowerUpManager::GetInstance()->GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &UStickyPaddleComponent::ReleaseBallFromPaddle, 2.0f, true); //Stick for 2 seconds
	}
}

void UStickyPaddleComponent::ReleaseBallFromPaddle()
{
	APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	if (BallInstance)
	{
		BallInstance->IsCurrentlySticked = false;
		BallInstance->StickedTo = nullptr;

		//Process has been completed, destroy this.
		APowerUpManager::GetInstance()->DestroySpawnedPowerUpComponent(this);
	}
}

void UStickyPaddleComponent::StopReceived()
{
	if (APowerUpManager::GetInstance())
	{
		APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}

	if (BallInstance)
	{
		BallInstance->IsCurrentlySticked = false;
		BallInstance->StickedTo = nullptr;
	}
}