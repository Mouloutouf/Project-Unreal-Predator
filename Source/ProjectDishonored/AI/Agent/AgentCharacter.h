// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgentVoice.h"
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
	UFUNCTION()
	void SetDestinationInfo(FPathPointInfo _PathPointInfo);
	
	UFUNCTION()
	void NextInPath();
	UFUNCTION()
	void ResetPath();
	UFUNCTION(BlueprintPure)
	bool CheckPathEnd() const;

	UFUNCTION()
	void CheckPlayerCanTakedown();
	UFUNCTION()
	void UpdatePlayerCanTakedown(bool _CanTakedown);
	UFUNCTION()
	void CheckPlayerCanConsume();
	UFUNCTION()
	void UpdatePlayerCanConsume(bool _CanConsume);

	UFUNCTION(BlueprintImplementableEvent)
	void TrySetAgentWeapon();

	UFUNCTION()
	void StopShooting() const;
	
	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAgentVoice* VoiceComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BlackboardKeyDeathState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentController* ControllerReference;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerCharacter* PlayerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsPathForward = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APathPoint* CurrentPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentPathIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool AtDestination = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPlayerTakedown = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPlayerConsume = false;

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
	int HiddenCapsulesCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AWeapon* Weapon = nullptr;
	
public:
	virtual void Tick(float _DeltaTime) override;

	UFUNCTION()
	void ChangeCharacterSpeed(float _NewSpeed) const;

	UFUNCTION()
	void EnableCharacter(bool _Enable);

	UFUNCTION(BlueprintCallable) // DEPRECATED
	void TryDeathByProjectile(AActor* _Other);
	
	UFUNCTION()
	void Death(FVector _HitDirection);

	bool GetIsDead() const { return IsDead; }
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanSeePlayer = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsAttackingPlayer = false;
};
