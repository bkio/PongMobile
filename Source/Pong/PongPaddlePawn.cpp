// Copyright 2016 Burak Kara - BurakIO.com

#include "Pong.h"

#include "PongBall.h"

#include "PowerUp/PowerUpManager.h"

#include "PongPaddlePawn.h"

APongPaddlePawn::APongPaddlePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CollisionStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("CollisionMeshComponent");
	CollisionStaticMeshComponent->SetCollisionProfileName(FName("Pawn"));
	CollisionStaticMeshComponent->SetVisibility(false);

	SpeedFactor = 1.0f;
	IsControllerReversed = false;
}

void APongPaddlePawn::Tick(float DeltaTime)
{
	APawn::Tick(DeltaTime);

	AddActorWorldOffset(Velocity * DeltaTime, true);
}

void APongPaddlePawn::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
#if PLATFORM_WINDOWS || PLATFORM_WINRT || PLATFORM_HTML5 || PLATFORM_MAC || PLATFORM_LINUX
	InputComponent->BindAction(FName("Start"), EInputEvent::IE_Pressed, this, &APongPaddlePawn::Start);
	InputComponent->BindAxis(FName("MoveRight"), this, &APongPaddlePawn::MoveRight);
//#elif PLATFORM_IOS || PLATFORM_ANDROID
#endif
}

void APongPaddlePawn::Start()
{
	APongBall* BallInstance = nullptr;
	for (TActorIterator<APongBall> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		ActorIterator->Start();
		BallInstance = *ActorIterator;
		break;
	}
	if (APowerUpManager::GetInstance() && BallInstance)
	{
		APowerUpManager::GetInstance()->StartReceived(BallInstance, this->GetActorLocation().Z);
	}

	if (IsThisForPlayer())
	{
		DisablePlayButton();
	}
}

void APongPaddlePawn::MoveRight(float Delta)
{
	Velocity = FVector(0.0f, Delta * (IsControllerReversed == true ? -1.0f : 1.0f) * 500.0f * SpeedFactor, 0.0f);
}

void APongPaddlePawn::SetSpeedFactor(float NewFactor)
{
	SpeedFactor = NewFactor;
}
float APongPaddlePawn::GetSpeedFactor()
{
	return SpeedFactor;
}
FVector APongPaddlePawn::GetCustomVelocity()
{
	return Velocity;
}

bool APongPaddlePawn::IsThisForPlayer()
{
	return this->AutoPossessPlayer == EAutoReceiveInput::Disabled ? false : true;
}

void APongPaddlePawn::SetVelocityZero()
{
	Velocity = FVector::ZeroVector;
}