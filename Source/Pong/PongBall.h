// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "Engine.h"
#include "PongPaddlePawn.h"

#include "PongBall.generated.h"

UCLASS()
class PONG_API APongBall : public AActor
{
	GENERATED_BODY()
	
public:
	APongBall();

	virtual void Tick(float) override;
	virtual void NotifyActorBeginOverlap(AActor*) override;

	void Start();

	float GetSphereRadius()
	{
		return SphereComponent->GetUnscaledSphereRadius();
	}

	bool IsCurrentlySticked;
	APongPaddlePawn* StickedTo;

	UFUNCTION(BlueprintImplementableEvent, Category = "Visibility")
		void SetBallRenderVisibility(bool NewVisibility);

	UFUNCTION(BlueprintImplementableEvent, Category = "Size")
		void ChangeBallSize(bool Shrink);

private:
	UPROPERTY(Category = Pong, VisibleAnywhere, Meta = (EditPrivateAccess = true))
	USphereComponent* SphereComponent;

	FVector Velocity;
	FVector InitialLocation;
};
