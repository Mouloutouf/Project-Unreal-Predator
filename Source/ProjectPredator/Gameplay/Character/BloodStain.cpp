// Fill out your copyright notice in the Description page of Project Settings.

#include "BloodStain.h"

ABloodStain::ABloodStain()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABloodStain::BeginPlay()
{
	Super::BeginPlay();
}

void ABloodStain::UpdateAge()
{
	// TODO Set the Age of the Stain each frame
	// Update the color of the stain based on the Age
	// Age should only decrease if the Player is not on the stain or if he is not dirty
}

void ABloodStain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAge();
}
