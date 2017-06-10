// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "PowerUpComponent.h"
#include "PowerUpManager.h"
#include "../PongBall.h"

void UPowerUpComponent::SetPowerUpComponent(APongBall* BallReference)
{
	BallInstance = BallReference;
}
void UPowerUpComponent::OnOverlap(class AActor * OtherActor, class UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor->IsA(APongBall::StaticClass()))
	{
		if (APowerUpManager::GetInstance())
		{
			//If child component wants to be destroyed after executing Behaviour, otherwise child component will have to destroy itself.
			if (Behaviour())
			{
				APowerUpManager::GetInstance()->DestroySpawnedPowerUpComponent(this);
			}
			else
			{
				APowerUpManager::GetInstance()->MarkSpawnedPowerUpComponentAsInactive(this);
			}
		}
	}
}
bool UPowerUpComponent::Behaviour()
{
	LastInteractedPaddle = APowerUpManager::GetInstance()->LastInteractedPaddle;
	return true;
}
void UPowerUpComponent::PaddleHitNotification(APongPaddlePawn* NewlyTouchedPaddle)
{
	//Overridable
}
void UPowerUpComponent::StopReceived()
{
	//Overridable
}