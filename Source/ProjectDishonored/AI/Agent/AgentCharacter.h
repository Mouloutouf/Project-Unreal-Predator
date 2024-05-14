// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectDishonored/AI/Navigation/PathPointInfo.h"
#include "ProjectDishonored/Gameplay/Items/Weapon.h"
#include "ProjectDishonored/Player/PlayerCharacter.h"
#include "AgentCharacter.generated.h"

class AAgentController;
class APath;

UCLASS()
class PROJECTDISHONORED_API AAgentCharacter : public ACharacter, public IHideableInterface
{
	GENERATED_BODY()

public:
	AAgentCharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Stop();
	
	UFUNCTION(BlueprintCallable)
	void SetNextDestination();
	UFUNCTION(BlueprintCallable)
	void SetDestinationInfo(FPathPointInfo _PathPointInfo);
	
	UFUNCTION(BlueprintCallable)
	void NextInPath();
	UFUNCTION(BlueprintCallable)
	void ResetPath();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool CheckPathEnd();

	UFUNCTION(BlueprintCallable)
	void CheckPlayerCanTakedown();

	UFUNCTION(BlueprintCallable)
	void CheckPlayerCanConsume();

	UFUNCTION(BlueprintCallable)
	void TryDeathByProjectile(AActor* _Other);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TrySetAgentWeapon();

	UFUNCTION()
	void TryStopShooting() const;
	
	//
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BlackboardKeyDeathState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentController* ControllerReference;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerCharacter* PlayerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsPathForward;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APathPoint* CurrentPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentPathIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool AtDestination;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanPlayerTakedown;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanPlayerConsume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HitForce = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> BonesToHitOnDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BodyCenterBone;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName HeadCenterBone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UCapsuleComponent*> DetectableCapsules;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int HiddenCapsulesCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AWeapon* Weapon;
	
public:
	virtual void Tick(float _DeltaTime) override;

	bool GetIsDead() const { return IsDead; }

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterSpeed(float _NewSpeed);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerCanTakedown(bool _CanTakedown);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerCanConsume(bool _CanConsume);
	
	UFUNCTION(BlueprintCallable)
	void EnableCharacter(bool _Enable);

	UFUNCTION(BlueprintCallable)
	void Death(FVector _HitDirection);

	UFUNCTION(BlueprintCallable)
	FVector GetBodyCenterLocation() const
	{
		return BodyCenterBone.IsNone() == false ? Mesh->GetBoneLocation(BodyCenterBone) : GetActorLocation();
	}

	UFUNCTION(BlueprintCallable)
	FVector GetHeadCenterLocation() const
	{
		return HeadCenterBone.IsNone() == false ? Mesh->GetBoneLocation(HeadCenterBone) : GetActorLocation();
	}

	AWeapon* GetWeapon() const { return Weapon; }
	
	virtual USceneComponent* GetCapsulesRoot() override { return Mesh; }
	
	virtual TArray<UCapsuleComponent*>& GetDetectableCapsules() override { return DetectableCapsules; }

	virtual int GetHiddenCapsulesCount() override { return HiddenCapsulesCount; }
	virtual void ChangeHiddenCapsulesCount(int _Delta) override { HiddenCapsulesCount += _Delta; }
	
	//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APath* Path;

	bool IsPlayerDetected;
	
	int CurrentSuspicionLevel;
};
