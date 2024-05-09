// Fill out your copyright notice in the Description page of Project Settings.

#include "HideableInterface.h"

void IHideableInterface::GatherDetectableCapsules()
{
	TArray<USceneComponent*> Children;
	GetCapsulesRoot()->GetChildrenComponents(true, Children);
	for (auto* Child : Children)
	{
		UCapsuleComponent* DetectableCapsule = Cast<UCapsuleComponent>(Child);
		if (DetectableCapsule != nullptr)
			GetDetectableCapsules().Add(DetectableCapsule);
	}
}

bool IHideableInterface::IsHidden()
{
	return GetHiddenCapsulesCount() >= GetDetectableCapsules().Num();
}
