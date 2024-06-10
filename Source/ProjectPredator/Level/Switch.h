// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "GameFramework/Actor.h"
#include "Switch.generated.h"

UCLASS()
class PROJECTPREDATOR_API ASwitch : public AActor
{
	GENERATED_BODY()
	
public:
	ASwitch();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UnlockDoor();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFeedbacks();
	
public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADoor* Door;
};
