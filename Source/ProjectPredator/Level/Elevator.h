// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Elevator.generated.h"

UCLASS()
class PROJECTPREDATOR_API AElevator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetActive(bool active);

	UFUNCTION(BlueprintCallable)
	void SwitchDirection();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsForward();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETimelineDirection::Type> Direction;
};
