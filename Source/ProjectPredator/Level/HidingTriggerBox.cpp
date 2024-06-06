// Fill out your copyright notice in the Description page of Project Settings.

#include "HidingTriggerBox.h"

#include "ProjectPredator/Gameplay/Character/HideableInterface.h"

AHidingTriggerBox::AHidingTriggerBox()
{
}

void AHidingTriggerBox::CheckEnter(AActor* _EnterActor, UPrimitiveComponent* _EnterComponent)
{
	Check(_EnterActor, _EnterComponent, true);
}

void AHidingTriggerBox::CheckExit(AActor* _ExitActor, UPrimitiveComponent* _ExitComponent)
{
	Check(_ExitActor, _ExitComponent, false);
}

void AHidingTriggerBox::Check(AActor* _Actor, const UPrimitiveComponent* _Component, const bool _Enter)
{
	IHideableInterface* HideableActor = Cast<IHideableInterface>(_Actor);
	if (HideableActor == nullptr)
		return;

	for (auto* DetectableCapsule : HideableActor->GetDetectableCapsules())
	{
		if (DetectableCapsule != _Component)
			continue;

		HideableActor->ChangeHiddenCapsulesCount(_Enter ? 1 : -1);
	}
}
