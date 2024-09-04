// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "PathPoint.generated.h"

class APath;

UCLASS()
class PROJECTPREDATOR_API APathPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	APathPoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float _DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WaitTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APath* AttachedPath;
};
