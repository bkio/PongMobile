// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "SpeedBoostComponent.h"
#include "../PowerUpManager.h"

USpeedBoostComponent::USpeedBoostComponent(const class FObjectInitializer& PCIP) : UPowerUpComponent(PCIP)
{
	PrimaryComponentTick.bCanEverTick = true;

	StartCounting_ForSpeedUpEasing = false;
	StartCounting_ForSpeedDown = -1.0f; //We use -1.0f as a false-like value
}

bool USpeedBoostComponent::Behaviour()
{
	UPowerUpComponent::Behaviour();

	if (LastInteractedPaddle == nullptr) return true;

	APowerUpManager::GetInstance()->PowerUpActivated(EPowerUpType::SpeedBoost, LastInteractedPaddle->AutoPossessPlayer != EAutoReceiveInput::Disabled);

	BeforeBoostSpeedFactorOfPaddle = LastInteractedPaddle->GetSpeedFactor();
	DesiredSpeedFactorOfPaddle = BeforeBoostSpeedFactorOfPaddle * 2.0f; //To it's double

	StartCounting_ForSpeedUpEasing = true;

	//Should not be destroyed now, but should be invisible.
	return false; 
}

void USpeedBoostComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	UPowerUpComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//If the ball hit this power-up
	if (StartCounting_ForSpeedUpEasing)
	{
		LastInteractedPaddle->SetSpeedFactor(FMath::InterpEaseOut<float>(LastInteractedPaddle->GetSpeedFactor(), DesiredSpeedFactorOfPaddle, DeltaTime * 2, 1.0f));

		if (FMath::IsNearlyEqual(LastInteractedPaddle->GetSpeedFactor(), DesiredSpeedFactorOfPaddle, 0.01f))
		{
			StartCounting_ForSpeedUpEasing = false;
			StartCounting_ForSpeedDown = 0.0f;
		}
	}
	//If speed is increased to it's double value, count for restoration
	if (StartCounting_ForSpeedDown >= 0.0f)
	{
		StartCounting_ForSpeedDown += DeltaTime;
		if (StartCounting_ForSpeedDown >= 5.0f) //Speed up for 5 seconds
		{
			StartCounting_ForSpeedDown = -1.0f; //Stop counting for speed down

			LastInteractedPaddle->SetSpeedFactor(BeforeBoostSpeedFactorOfPaddle); //Restore back to it's original value

			//Process has been completed, destroy this.
			if (APowerUpManager::GetInstance())
			{
				APowerUpManager::GetInstance()->DestroySpawnedPowerUpComponent(this);
			}
		}
	}
}