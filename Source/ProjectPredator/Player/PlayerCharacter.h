// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectPredator/Gameplay/Character/HideableInterface.h"
#include "ProjectPredator/UI/PlayerHUD.h"
#include "PlayerCharacter.generated.h"

class AAgentCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnergyChangedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeathSignature);

UCLASS()
class PROJECTPREDATOR_API APlayerCharacter : public ACharacter, public IHideableInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void EnableAbilities(bool _Enable);

	UFUNCTION()
	void EnableSprint(bool _Enable);
	UFUNCTION()
	void ActivateSprint(bool _Activate);
	
	UFUNCTION()
	void SetProne(float _VignetteIntensity, float _NewHeight) const;
	UFUNCTION()
	void ActivateProne(bool _Activate, bool _ShouldAnimate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	// TODO Implement this method or Remove the feature altogether
	void ProneAnimation(bool _Activate);
	
	UFUNCTION()
	void SetSpeed(float _NewSpeed) const;

	UFUNCTION()
	void SetHealth(float _NewHealth);
	UFUNCTION()
	void SetEnergy(float _NewEnergy);
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealthUI();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnergyUI();
	
	UFUNCTION(BlueprintCallable)
	void EnableLoadingAnimation(bool _Enable);

	UFUNCTION()
	void TryTakedown();
	UFUNCTION()
	void InitiateTakedown();
	UFUNCTION()
	void InitiateKill();
	UFUNCTION(BlueprintImplementableEvent)
	void KillAnimation(float _Rate);
	UFUNCTION(BlueprintCallable)
	void FinishTakedown();

	UFUNCTION()
	void TryConsumeBody();
	UFUNCTION()
	void InitiateConsumeBody();
	UFUNCTION(BlueprintImplementableEvent)
	void ConsumeAnimation(float _Rate);
	UFUNCTION(BlueprintCallable)
	void FinishConsumeBody();

	UFUNCTION()
	void SwitchVision();
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchVisionAnimation(bool _Activate);
	
	UFUNCTION()
	void TryMakeNoise();

	UFUNCTION()
	void UpdateRaycastAndReticle();

	UFUNCTION()
	void UpdateEnergyAndHealth();

	UFUNCTION()
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

	void VisionPressed();
	
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
	bool CanPerformProne = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformSprint = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPerformTakedown = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanSwitchVision = true;

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
	bool VisionActive;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StandingHeight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CrouchingHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float NormalSpeed = 100;
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
	bool HealthDecreaseStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownDetectedHealthDecrease;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxEnergy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultEnergyDecreaseRate = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VisionActiveEnergyDecreaseRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovementEnergyDecreaseRate = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SprintEnergyDecreaseRate = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConsumeEnergyIncrease;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ConsumeHealthIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageEnergyDecrease;
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* CurrentAgentInTakedownRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* CurrentDeadAgentInRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownAgentOffsetDistance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownMoveSpeed = 2500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentTakedownDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TakedownRagdollForce = 10000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UCapsuleComponent*> DetectableCapsules;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int HiddenCapsulesCount;
	
public:
	virtual void SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent) override;
	
	virtual void Tick(float _DeltaTime) override;

	// Readonly Components
	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }
	USceneComponent* GetTakedownRoot() const { return TakedownRoot; }

	// Readonly Status Booleans
	bool GetIsInProne() const { return IsInProne; }
	bool GetIsSprinting() const { return IsSprinting; }
	bool GetIsInTakedown() const { return IsInTakedown; }
	bool GetCanPerformTakedown() const { return CanPerformTakedown; }
	bool GetIsEating() const { return IsEating; }
	bool GetIsDead() const { return IsDead; }
	bool GetHealthDecreaseStatus() const { return HealthDecreaseStatus; }

	FVector GetCameraForward() const
	{
		return UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0));
	}
	
	bool GetIsMoving() const
	{
		return CharacterMovement->Velocity.Size() > 0;
	}

	UFUNCTION()
	void ChangeHealth(float _HealthChange);
	UFUNCTION()
	void ChangeEnergy(float _EnergyChange);
	UFUNCTION()
	void ChangeEnergyOnDamage();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetTakedownWidgetVisible(bool _Visible);
	UFUNCTION(BlueprintImplementableEvent)
	void SetConsumeWidgetVisible(bool _Visible);

	UFUNCTION()
	void AddAgentForTakedown(AAgentCharacter* _Agent);
	UFUNCTION()
	void RemoveAgentFromTakedown(const AAgentCharacter* _Agent);
	UFUNCTION()
	void AddDeadAgent(AAgentCharacter* _Agent);
	UFUNCTION()
	void RemoveDeadAgent(const AAgentCharacter* _Agent);

	virtual USceneComponent* GetCapsulesRoot() override { return Mesh; }
	virtual TArray<UCapsuleComponent*>& GetDetectableCapsules() override { return DetectableCapsules; }
	virtual int GetHiddenCapsulesCount() override { return HiddenCapsulesCount; }
	virtual void ChangeHiddenCapsulesCount(int _Delta) override { HiddenCapsulesCount += _Delta; }
	
	//

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnEnergyChangedSignature OnEnergyChanged;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnPlayerDeathSignature OnPlayerDeath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<AAgentCharacter*> AgentsInTakedownRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<AAgentCharacter*> DeadAgentsInRange;

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
