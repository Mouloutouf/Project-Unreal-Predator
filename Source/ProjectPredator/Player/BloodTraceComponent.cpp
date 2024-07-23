// Fill out your copyright notice in the Description page of Project Settings.

#include "BloodTraceComponent.h"

UBloodTraceComponent::UBloodTraceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBloodTraceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBloodTraceComponent::SpawnBloodStain()
{
	// TODO Spawn the Blood Stain Prefab
	// Set the opacity of the Blood Stain based on the Dirt Level
	// Add it to a linked list of stains stored in the world representing a single trace
}

void UBloodTraceComponent::UpdateBloodTrace()
{
	// TODO Call the Spawn Blood Stain function every time the Player leaves the collider range of another stain or if that stain becomes dry
	// Make the Dirt Level decrease for each spawn by the Dirt Decrease Value (should factor in the distance and the potential multipliers)
}

void UBloodTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateBloodTrace();
}
