// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "Turret.generated.h"

UCLASS()
class PROJECTPREDATOR_API ATurret : public AGun
{
	GENERATED_BODY()
	
public:
	ATurret();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	AActor* CurrentTarget;

	// TODO Implement the threat interface and make sure the threat status is modified on death
	// We are then going to use this to indicate to our turret that the target is not a threat anymore and it can therefore select a new target

	// TODO Implement the cone of vision with dot product using a capsule collider with a height check, debug draw during play (or in editor if you find out how to do it)
	// Then selection of target based on how close the target is to the current direction of shoot using a dot product again

	// TODO Create a model for the turret in blueprint, separate the base from the gun part for rotation, then implement the rotation system with two parts
	// Then make sure it can aim at a target position using both rotations

	// TODO Implement the feedbacks and sequence of actions, the turret should only shoot when its barrel is aligned with target (with a delta)
};
