// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "PathPoint.generated.h"

class APath;

UCLASS()
class PROJECTDISHONORED_API APathPoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float _DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WaitTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APath* AttachedPath;
};
