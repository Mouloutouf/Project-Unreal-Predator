// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crossbow.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "ProjectDishonored/UI/PlayerHUD.h"
#include "PlayerCharacter.generated.h"

class AAgentCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnergyChangedSignature);

UCLASS()
class PROJECTDISHONORED_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void EnableAbilities(bool _Enable);

	UFUNCTION(BlueprintCallable)
	void EnableSprint(bool _Enable);

	UFUNCTION(BlueprintCallable)
	void ActivateSprint(bool _Activate);
	
	UFUNCTION(BlueprintCallable)
	void SetCrouch(float _VignetteIntensity, float _NewHeight);

	UFUNCTION(BlueprintCallable)
	void ActivateCrouch(bool _Activate, bool _ShouldAnimate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CrouchAnimation(bool _Activate);
	
	UFUNCTION(BlueprintCallable)
	void SetSpeed(float _NewSpeed);

	UFUNCTION(BlueprintCallable)
	void SetHealth(float _NewHealth);

	UFUNCTION(BlueprintCallable)
	void ChangeHealth(float _HealthChange);

	UFUNCTION(BlueprintCallable)
	void SetEnergy(float _NewEnergy);

	UFUNCTION(BlueprintCallable)
	void ChangeEnergy(float _EnergyChange);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateHealthUI();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateEnergyUI();
	
	UFUNCTION(BlueprintCallable)
	void TryGrabAgent();

	UFUNCTION(BlueprintCallable)
	void TryReleaseAgent();
	
	UFUNCTION(BlueprintCallable)
	void EnableTakedownUI(bool _Enable);

	UFUNCTION(BlueprintCallable)
	void InitiateTakedown();

	UFUNCTION(BlueprintCallable)
	void FinishTakedown();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TakedownAnimation();
	
	UFUNCTION(BlueprintCallable)
	void TryMakeNoise();

	UFUNCTION(BlueprintCallable)
	void UpdateRaycastAndReticle();

	UFUNCTION(BlueprintCallable)
	void UpdateIsMovingForward(bool _State);

	UFUNCTION(BlueprintCallable)
	void DecreaseEnergy();

	UFUNCTION(BlueprintCallable)
	void DecreaseHealth();

	UFUNCTION(BlueprintCallable)
	void CheckEnergy();

	UFUNCTION(BlueprintCallable)
	void TryDecreaseHealth();

	UFUNCTION(BlueprintCallable)
	void Die();
	
private:
	void MoveForward(float _AxisValue);
	void MoveRight(float _AxisValue);

	// Controller Input
	void TurnRate(float _AxisValue);
	void LookUpRate(float _AxisValue);

	// Mouse Input
	void Turn(float _AxisValue);
	void LookUp(float _AxisValue);

	void JumpPressed();
	void JumpReleased();

	void ShootPressed();

	void SprintPressed();
	void SprintReleased();

	void CrouchPressed();

	void TakedownPressed();
	
public:	
	// Called every frame
	virtual void Tick(float _DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;

	bool GetIsCrouching() const { return IsCrouching; }
	bool GetIsSprinting() const { return IsSprinting; }
	bool GetIsInTakedown() const { return IsInTakeDown; }
	bool GetIsMovingForward() const { return IsMovingForward; }
	
	bool GetCanPerformTakedown() const { return CanPerformTakedown; }

	bool GetIsDead() const { return IsDead; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChildActorComponent* ChildCrossbow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* TakedownRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformJump = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformCrouch = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformSprint = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformTakedown = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsInTakeDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsMovingForward;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StandingHeight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CrouchingHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlowSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CrouchingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int HealthDecreaseSteps = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HealthDecreaseWaitTime = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float HealthDecreaseCurrentTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ShouldDecreaseHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxEnergy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float CurrentEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EnergyDecreaseSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownEnergyIncrease;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ControllerXSensitivity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ControllerYSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultVignetteIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACrossbow* CrossbowReference;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerHUD* HUDReference;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController* ControllerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentHitPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AgentBodyDropOffsetDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* CurrentGrabbedAgent;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnEnergyChangedSignature OnEnergyChanged;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AAgentCharacter* CurrentAgentInRange;
};
