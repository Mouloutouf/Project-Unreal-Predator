// Fill out your copyright notice in the Description page of Project Settings.


#include "HidingTriggerBox.h"

#include "ProjectDishonored/Player/PlayerCharacter.h"

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
	APlayerCharacter* PlayerCharacter = dynamic_cast<APlayerCharacter*>(_Actor);
	
	if (PlayerCharacter == nullptr)
		return;

	if ((UPrimitiveComponent*)PlayerCharacter->GetHidingTrigger() != _Component)
		return;

	PlayerCharacter->SetHiddenStatus(_Enter);
}
