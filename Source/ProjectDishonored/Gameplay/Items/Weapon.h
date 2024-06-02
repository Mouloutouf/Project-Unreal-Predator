// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AAgentCharacter;

UCLASS()
class PROJECTDISHONORED_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* AgentOwner;
	
	virtual void BeginPlay() override;

public:
	virtual void Tick(float _DeltaTime) override;

	AAgentCharacter* GetWeaponOwner() const { return AgentOwner; }
	void SetWeaponOwner(AAgentCharacter* _AgentOwner) { AgentOwner = _AgentOwner; }
};
