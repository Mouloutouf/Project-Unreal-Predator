// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BloodStain.generated.h"

UCLASS()
class PROJECTPREDATOR_API ABloodStain : public AActor
{
	GENERATED_BODY()
	
public:
	ABloodStain();

protected:
	virtual void BeginPlay() override;

	void UpdateAge();
	
public:
	virtual void Tick(float DeltaTime) override;

	bool IsDry() const { return Age >= MaximumAge; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Age;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaximumAge;
};
