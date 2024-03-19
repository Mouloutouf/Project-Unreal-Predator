// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameInfoInstance.generated.h"

UCLASS()
class PROJECTDISHONORED_API UGameInfoInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UGameInfoInstance();

	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowMainMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowHostMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowServerMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowOptionsMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowLoadingScreen();
	
	// UFUNCTION(BlueprintCallable)
	// void TryLaunchLobby(int _NumberOfPlayers, bool _EnableLan, FString _ServerName);
	//
	// UFUNCTION()
	// void LaunchLobby();
	// UFUNCTION()
	// void FailedToLaunchLobby();
	//
	// UFUNCTION(BlueprintCallable)
	// void TryJoinServer(FBlueprintSessionResult _SessionToJoin);
	//
	// UFUNCTION()
	// void FailedToJoinServer();
	//
	// UFUNCTION()
	// void DisplayErrorMessage(FString _ErrorMessage);

// protected:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
// 	int MaxPlayers;
//
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
// 	FString ServerName;
};
