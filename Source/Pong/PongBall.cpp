// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"
#include "PongPaddlePawn.h"
#include "PongBall.h"
#include "PowerUp/PowerUpManager.h"

APongBall::APongBall()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComponent->SetSphereRadius(50.0f);
	SphereComponent->SetCollisionProfileName(FName("PhysicsActor"));
	SphereComponent->SetNotifyRigidBodyCollision(true);

	IsCurrentlySticked = false;
}

void APongBall::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	if (IsCurrentlySticked)
	{
		AddActorWorldOffset(StickedTo->GetCustomVelocity() * DeltaTime, true);
	}
	else
	{
		FHitResult HitResult;
		AddActorWorldOffset(Velocity * DeltaTime, true, &HitResult);
		AddActorWorldRotation(FRotator(-Velocity.X, 0.0f, Velocity.Y) * DeltaTime);

		if (HitResult.GetActor())
		{
			if (HitResult.GetActor()->IsA(APongPaddlePawn::StaticClass()))
			{
				if (APowerUpManager::GetInstance())
				{
					APowerUpManager::GetInstance()->InteractedWithPaddle((APongPaddlePawn*)HitResult.GetActor());
				}
			}

			Velocity = Velocity - 2.0f * (FVector::DotProduct(HitResult.Normal, Velocity)) * HitResult.Normal;
			Velocity.Z = 0.0f;

			if (HitResult.GetActor()->GetClass()->IsChildOf(APongPaddlePawn::StaticClass()))
			{
				Velocity *= 1.02f;
			}
		}
	}
}

void APongBall::NotifyActorBeginOverlap(AActor* Actor)
{
	for (TActorIterator<APongPaddlePawn> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		if (ActorIterator->ScoreTriggerVolume == Actor)
		{
			SetActorLocation(InitialLocation);
			Velocity = FVector::ZeroVector;

			ActorIterator->Score += 1.0f;

			if (APowerUpManager::GetInstance())
			{
				APowerUpManager::GetInstance()->StopReceived();
			}

			if (ActorIterator->IsThisForPlayer())
			{
				ActorIterator->EnablePlayButton();
			}
			else
			{
				for (TActorIterator<APongPaddlePawn> OtherIterator(GetWorld()); OtherIterator; ++OtherIterator)
				{
					if (OtherIterator->IsThisForPlayer())
					{
						OtherIterator->EnablePlayButton();
						break;
					}
				}
			}
			break;
		}
	}
}

void APongBall::Start()
{
	if (FMath::IsNearlyZero(Velocity.Size()))
	{
		InitialLocation = GetActorLocation();
		float Angle = FMath::FRandRange(-50.0f, 50.0f);
		float Direction = FMath::RandBool() ? -1.0f : 1.0f;
		float FinalAngle = FMath::DegreesToRadians(Direction * 90.0f + Angle);
		Velocity = FVector(FMath::Sin(FinalAngle), FMath::Cos(FinalAngle), 0.0f) * 750.0f; 
	}
}