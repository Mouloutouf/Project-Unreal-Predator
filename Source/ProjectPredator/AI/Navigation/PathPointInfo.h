// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathPoint.h"
#include "PathPointInfo.generated.h"

USTRUCT(BlueprintType)
struct PROJECTPREDATOR_API FPathPointInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APathPoint* PathPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;
};
