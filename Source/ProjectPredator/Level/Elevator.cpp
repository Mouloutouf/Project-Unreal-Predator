// Fill out your copyright notice in the Description page of Project Settings.

#include "Elevator.h"

AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElevator::SetActive(bool active)
{
	IsActive = active;
}

void AElevator::SwitchDirection()
{
	Direction = Direction == ETimelineDirection::Type::Forward ? ETimelineDirection::Type::Backward : ETimelineDirection::Type::Forward;
}

bool AElevator::IsForward()
{
	return Direction == ETimelineDirection::Type::Forward;
}
