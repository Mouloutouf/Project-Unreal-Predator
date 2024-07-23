// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectPredator/Gameplay/Character/BloodStain.h"
#include "BloodTraceComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTPREDATOR_API UBloodTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBloodTraceComponent();

protected:
	virtual void BeginPlay() override;

	void SpawnBloodStain();

	void UpdateBloodTrace();
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool IsDirty() { return DirtLevel > 0; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DirtLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DirtDecreaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DirtDecreaseHidingAreaMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DistanceBetweenEachStain;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ABloodStain* BloodStain;
};
