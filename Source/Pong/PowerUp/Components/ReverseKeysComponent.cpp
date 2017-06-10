// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "ReverseKeysComponent.h"
#include "../PowerUpManager.h"

bool UReverseKeysComponent::Behaviour()
{
	UPowerUpComponent::Behaviour();

	if (LastInteractedPaddle == nullptr) return true;

	APowerUpManager::GetInstance()->PowerUpActivated(EPowerUpType::ReverseKeys, LastInteractedPaddle->AutoPossessPlayer != EAutoReceiveInput::Disabled);

	LastInteractedPaddle->IsControllerReversed = true;

	//Stay reversed for 5 or 1.5 seconds, for player it is 5, for AI it is 1.5.
	APowerUpManager::GetInstance()->GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &UReverseKeysComponent::RestoreControllerBack, 
		LastInteractedPaddle->AutoPossessPlayer == EAutoReceiveInput::Disabled ? 1.5f : 5.0f, true);

	//Should not be destroyed now, but should be invisible.
	return false;
}

void UReverseKeysComponent::RestoreControllerBack()
{
	APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	LastInteractedPaddle->IsControllerReversed = false;

	//Process has been completed, destroy this.
	APowerUpManager::GetInstance()->DestroySpawnedPowerUpComponent(this);
}

void UReverseKeysComponent::StopReceived()
{
	if (APowerUpManager::GetInstance())
	{
		APowerUpManager::GetInstance()->GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}
	
	if (LastInteractedPaddle)
	{
		LastInteractedPaddle->IsControllerReversed = false;
	}
}