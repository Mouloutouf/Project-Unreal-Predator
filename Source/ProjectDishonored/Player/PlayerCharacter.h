// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	void SetProne(float _VignetteIntensity, float _NewHeight);

	UFUNCTION(BlueprintCallable)
	void ActivateProne(bool _Activate, bool _ShouldAnimate);

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
	void EnableLoadingAnimation(bool _Enable);

	UFUNCTION(BlueprintCallable)
	void InitiateTakedown();

	UFUNCTION(BlueprintCallable)
	void TakedownKill();
	
	UFUNCTION(BlueprintCallable)
	void FinishTakedown();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void KillAnimation(float _Rate);

	UFUNCTION(BlueprintCallable)
	void InitiateConsumeBody();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ConsumeAnimation(float _Rate);

	UFUNCTION(BlueprintCallable)
	void FinishConsumeBody();
	
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

	void SprintPressed();
	void SprintReleased();

	void PronePressed();

	void TakedownPressed();

	void EatPressed();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* TakedownRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanMove = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanLook = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformJump = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformCrouch = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformSprint = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformTakedown = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsInProne;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PerformTakedownMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsInTakedown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsEating;
	
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
	float ProneSpeed;
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
	float TakedownDetectedHealthDecrease;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxEnergy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	float CurrentEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EnergyImmobileDecreaseSpeed = 0.27;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EnergyMovementDecreaseSpeed = 1.67;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EnergySprintDecreaseFactor = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConsumeEnergyIncrease;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ControllerXSensitivity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ControllerYSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultVignetteIntensity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerHUD* HUDReference;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController* ControllerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentHitPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownAgentOffsetDistance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownMoveSpeed = 2500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentTakedownDirection;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnEnergyChangedSignature OnEnergyChanged;
	
public:
	// Called every frame
	virtual void Tick(float _DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;

	bool GetIsCrouching() const { return IsInProne; }
	bool GetIsSprinting() const { return IsSprinting; }
	bool GetIsInTakedown() const { return IsInTakedown; }
	bool GetIsEating() const { return IsEating; }
	bool GetIsMovingForward() const { return IsMovingForward; }

	bool GetIsMoving() const { return CharacterMovement->Velocity.Size() > 0; }
	
	bool GetCanPerformTakedown() const { return CanPerformTakedown; }

	bool GetIsDead() const { return IsDead; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTakedownWidgetVisible(bool _Visible);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetConsumeWidgetVisible(bool _Visible);
	
	//
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AAgentCharacter* CurrentAgentInKillRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AAgentCharacter* CurrentDeadAgentInRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UndetectedKillRadius = 200;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DetectedKillRadius = 400;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillHeightRange = 80;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConsumeRadius = 150;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConsumeHeightRange = 80;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DetectedKillNormalAnimationRate = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DetectedKillSlowAnimationRate = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConsumeAnimationRate = 0.5;
};
