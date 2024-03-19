// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectDishonored/AI/Navigation/PathPointInfo.h"
#include "ProjectDishonored/Player/PlayerCharacter.h"
#include "AgentCharacter.generated.h"

class AAgentController;
class APath;

UCLASS()
class PROJECTDISHONORED_API AAgentCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAgentCharacter();

protected:
	// Called when the game starts or when spawned
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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTakedownUIVisible(bool _Visible);
	UFUNCTION(BlueprintCallable)
	void CheckPlayerCanTakedown();

	UFUNCTION(BlueprintCallable)
	void TryDeathByProjectile(AActor* _Other);
	
	//
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsDead;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HitForce = 10000;
	
public:
	// Called every frame
	virtual void Tick(float _DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;

	bool GetIsDead() const { return IsDead; }
	APlayerCharacter* GetPlayer() const { return PlayerReference; }

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterSpeed(float _NewSpeed);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerCanTakedown(bool _CanTakedown);
	
	UFUNCTION(BlueprintCallable)
	void EnableCharacter(bool _Enable);

	UFUNCTION(BlueprintCallable)
	void Death(FVector _HitDirection);

	//
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	APath* Path;

	bool IsPlayerDetected;
	int CurrentSuspicionLevel;
};
