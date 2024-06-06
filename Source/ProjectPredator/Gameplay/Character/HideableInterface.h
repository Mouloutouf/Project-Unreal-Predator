// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "UObject/Interface.h"
#include "HideableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHideableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTPREDATOR_API IHideableInterface
{
	GENERATED_BODY()

protected:
	void GatherDetectableCapsules();
	
public:
	// TODO / Use Tags or something to denote which elements from under the Mesh are Detectable Capsules
	virtual USceneComponent* GetCapsulesRoot() = 0;
	
	virtual TArray<UCapsuleComponent*>& GetDetectableCapsules() = 0;

	virtual int GetHiddenCapsulesCount() = 0;
	virtual void ChangeHiddenCapsulesCount(int _Delta) = 0;

	bool IsHidden();
};
