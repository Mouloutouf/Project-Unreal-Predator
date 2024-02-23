// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "GameFramework/Actor.h"
#include "Switch.generated.h"

UCLASS()
class PROJECTDISHONORED_API ASwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UnlockDoor();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFeedbacks();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ADoor* Door;
};
