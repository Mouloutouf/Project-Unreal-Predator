// Fill out your copyright notice in the Description page of Project Settings.


#include "Path.h"

// Sets default values
APath::APath()
{
}

// Called when the game starts or when spawned
void APath::BeginPlay()
{
	Super::BeginPlay();

	for (FPathPointInfo& PathPointInfo : PathPoints)
		PathPointInfo.PathPoint->AttachedPath = this;
}

// Called every frame
void APath::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

