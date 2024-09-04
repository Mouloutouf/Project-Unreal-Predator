// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PathPointInfo.h"
#include "GameFramework/Actor.h"
#include "Path.generated.h"

UENUM()
enum EPathBehavior
{
	Stop,
	Loop,
	Reverse
};

UCLASS()
class PROJECTPREDATOR_API APath : public AActor
{
	GENERATED_BODY()
	
public:	
	APath();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float _DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Forward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EPathBehavior> PathBehavior;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPathPointInfo> PathPoints;
};
