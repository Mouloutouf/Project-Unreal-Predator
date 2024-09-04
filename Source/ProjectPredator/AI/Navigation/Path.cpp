// Fill out your copyright notice in the Description page of Project Settings.

#include "Path.h"

APath::APath()
{
}

void APath::BeginPlay()
{
	Super::BeginPlay();

	for (FPathPointInfo& PathPointInfo : PathPoints)
		PathPointInfo.PathPoint->AttachedPath = this;
}

void APath::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}
