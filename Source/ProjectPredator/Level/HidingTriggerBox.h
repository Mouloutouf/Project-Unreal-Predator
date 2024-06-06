// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "HidingTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTPREDATOR_API AHidingTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AHidingTriggerBox();
	
protected:
	UFUNCTION(BlueprintCallable)
	void CheckEnter(AActor* _EnterActor, UPrimitiveComponent* _EnterComponent);
	UFUNCTION(BlueprintCallable)
	void CheckExit(AActor* _ExitActor, UPrimitiveComponent* _ExitComponent);

	static void Check(AActor* _Actor, const UPrimitiveComponent* _Component, bool _Enter);
};
