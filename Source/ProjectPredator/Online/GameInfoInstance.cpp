// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInfoInstance.h"

// #include "CreateSessionCallbackProxy.h"
// #include "Kismet/GameplayStatics.h"

UGameInfoInstance::UGameInfoInstance()
{
}

void UGameInfoInstance::Init()
{
	Super::Init();
}

void UGameInfoInstance::Shutdown()
{
	Super::Shutdown();
}

void UGameInfoInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

// void UObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
// }

// void UGameInfoInstance::TryLaunchLobby(int _NumberOfPlayers, bool _EnableLan, FString _ServerName)
// {
// 	MaxPlayers = _NumberOfPlayers;
// 	ServerName = _ServerName;
//
// 	ShowLoadingScreen();
//
// 	UCreateSessionCallbackProxy* CreateSessionCallbackProxy = UCreateSessionCallbackProxy::CreateSession(this, UGameplayStatics::GetPlayerController(this, 0), MaxPlayers, _EnableLan);
// 	CreateSessionCallbackProxy->OnSuccess.AddDynamic(this, &UGameInfoInstance::LaunchLobby);
// 	CreateSessionCallbackProxy->OnFailure.AddDynamic(this, &UGameInfoInstance::FailedToLaunchLobby);
// 	CreateSessionCallbackProxy->Activate();
// }
//
// void UGameInfoInstance::LaunchLobby()
// {
// 	UGameplayStatics::OpenLevel(this, "Lobby", true, "listen");
// }
//
// void UGameInfoInstance::FailedToLaunchLobby()
// {
// 	DisplayErrorMessage("Failed to Create Session. Please Try Again.");
// }
//
// void UGameInfoInstance::TryJoinServer(FBlueprintSessionResult _SessionToJoin)
// {
// 	ShowLoadingScreen();
//
// 	UJoinSessionCallbackProxy* JoinSessionCallbackProxy = UJoinSessionCallbackProxy::JoinSession(this, UGameplayStatics::GetPlayerController(this, 0), _SessionToJoin);
// 	JoinSessionCallbackProxy->OnFailure.AddDynamic(this, &UGameInfoInstance::FailedToJoinServer);
// 	JoinSessionCallbackProxy->Activate();
// }
//
// void UGameInfoInstance::FailedToJoinServer()
// {
// 	DisplayErrorMessage("Failed to Join Session. Please Try Again.");
// }
//
// void UGameInfoInstance::DisplayErrorMessage(FString _ErrorMessage)
// {
// }
