// Fill out your copyright notice in the Description page of Project Settings.

#include "TracesContainer.h"

ATracesContainer::ATracesContainer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATracesContainer::BeginPlay()
{
	Super::BeginPlay();
}

void ATracesContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
