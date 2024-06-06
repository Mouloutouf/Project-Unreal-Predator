// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTPREDATOR_API FPlayerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PlayerStatus;
};
