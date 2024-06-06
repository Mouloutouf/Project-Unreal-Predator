// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPREDATOR_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

	UPlayerSaveGame();
	
protected:
	// UPROPERTY()
	// FPlayerInfo PlayerInfo;
};
