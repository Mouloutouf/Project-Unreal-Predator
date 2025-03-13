// Fill out your copyright notice in the Description page of Project Settings.

#include "Turret.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
