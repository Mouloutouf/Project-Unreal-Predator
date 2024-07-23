// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TracesContainer.generated.h"

UCLASS()
class PROJECTPREDATOR_API ATracesContainer : public AActor
{
	GENERATED_BODY()
	
public:
	// TODO Make this a Singleton
	ATracesContainer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// TODO Implement an Array of Linked Lists to store the different Traces in the world
};
