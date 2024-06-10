// Fill out your copyright notice in the Description page of Project Settings.

#include "Switch.h"

ASwitch::ASwitch()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASwitch::BeginPlay()
{
	Super::BeginPlay();
}

void ASwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASwitch::UnlockDoor()
{
	if (Door == nullptr || Door->Unlocked == true)
		return;

	Door->Unlocked = true;
	Door->OnLockUpdated(true);

	PlayFeedbacks();
}
