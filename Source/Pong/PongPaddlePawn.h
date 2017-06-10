// Copyright 2016 Burak Kara - BurakIO.com

#pragma once

#include "Engine.h"

#include "PongPaddlePawn.generated.h"

UCLASS()
class PONG_API APongPaddlePawn : public APawn
{
	GENERATED_BODY()

public:
	APongPaddlePawn();

	virtual void Tick(float) override;

	UPROPERTY(Category = Pong, EditAnywhere)
	ATriggerVolume* ScoreTriggerVolume;

	UPROPERTY(Category = Pong, BlueprintReadOnly)
	int32 Score;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pong)
	float SpeedFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pong)
	bool IsControllerReversed;

	float GetSpeedFactor();
	void SetSpeedFactor(float NewFactor);

	FVector GetCustomVelocity();

	UFUNCTION(BlueprintCallable, Category = Pong)
	void Start();

	UFUNCTION(BlueprintCallable, Category = Pong)
	void MoveRight(float Delta);

	UFUNCTION(BlueprintCallable, Category = Pong)
	bool IsThisForPlayer();

	UFUNCTION(BlueprintCallable, Category = Pong)
	void SetVelocityZero();

	UFUNCTION(BlueprintImplementableEvent, Category = Pong)
	void EnablePlayButton();

	UFUNCTION(BlueprintImplementableEvent, Category = Pong)
	void DisablePlayButton();

private:
	UPROPERTY(Category = Pong, VisibleAnywhere, Meta = (EditPrivateAccess = true))
	UStaticMeshComponent* CollisionStaticMeshComponent;

	FVector Velocity;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent*) override;
	
};
