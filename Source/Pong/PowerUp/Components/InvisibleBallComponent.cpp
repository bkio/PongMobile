// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "InvisibleBallComponent.h"
#include "../PowerUpManager.h"

bool UInvisibleBallComponent::Behaviour()
{
	UPowerUpComponent::Behaviour();

	if (LastInteractedPaddle == nullptr) return true;

	APowerUpManager::GetInstance()->PowerUpActivated(EPowerUpType::InvisibleBall, LastInteractedPaddle->AutoPossessPlayer != EAutoReceiveInput::Disabled);

	//Set it invisible
	BallInstance->SetBallRenderVisibility(false);

	//Set ball invisible for 3 seconds if player hit this, otherwise slow AI down for 1.5 seconds and make it invisible for 1.5 seconds
	APowerUpManager::GetInstance()->GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &UInvisibleBallComponent::SetBallVisibleAgain,
		LastInteractedPaddle->AutoPossessPlayer == EAutoReceiveInput::Disabled ? 1.5f : 3.0f, true);

	//If paddle is controlled by AI
	if (LastInteractedPaddle->AutoPossessPlayer == EAutoReceiveInput::Disabled)
	{
		AIController_OldSpeedFactor = LastInteractedPaddle->GetSpeedFactor();
		LastInteractedPaddle->SetSpeedFactor(AIController_OldSpeedFactor / 2); //Slow down
	}
	//Should not be destroyed now, but should be invisible.
	return false;
}

void UInvisibleBallComponent::SetBallVisibleAgain()
{
	APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	//Set it visible
	BallInstance->SetBallRenderVisibility(true);

	//If paddle is controlled by AI
	if (LastInteractedPaddle->AutoPossessPlayer == EAutoReceiveInput::Disabled)
	{
		LastInteractedPaddle->SetSpeedFactor(AIController_OldSpeedFactor);
	}

	//Process has been completed, destroy this.
	APowerUpManager::GetInstance()->DestroySpawnedPowerUpComponent(this);
}

void UInvisibleBallComponent::StopReceived()
{
	if (APowerUpManager::GetInstance())
	{
		APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}
	
	//Set it visible
	if (BallInstance)
	{
		BallInstance->SetBallRenderVisibility(true);
	}

	//If paddle is controlled by AI
	if (LastInteractedPaddle)
	{
		if (LastInteractedPaddle->AutoPossessPlayer == EAutoReceiveInput::Disabled)
		{
			LastInteractedPaddle->SetSpeedFactor(AIController_OldSpeedFactor);
		}
	}
}