// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "ProjectDishonored/ProjectDishonored.h"
#include "ProjectDishonored/AI/Agent/AgentCharacter.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	TakedownRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TakedownRoot"));
	TakedownRoot->SetupAttachment(RootComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GatherDetectableCapsules();
	
	CanPerformJump = CanPerformProne = CanPerformSprint = CanPerformTakedown = true;

	ControllerReference = dynamic_cast<APlayerController*>(GetController());
	
	HUDReference = dynamic_cast<APlayerHUD*>(ControllerReference->GetHUD());
	HUDReference->ShouldDrawReticle = true;

	StandingHeight = Camera->GetRelativeLocation().Z;
	Camera->PostProcessSettings.VignetteIntensity = DefaultVignetteIntensity;
	
	OnHealthChanged.AddDynamic(this, &APlayerCharacter::UpdateHealthUI);
	OnEnergyChanged.AddDynamic(this, &APlayerCharacter::UpdateEnergyUI);

	SetHealth(MaxHealth);
	SetEnergy(MaxEnergy);

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	ControllerReference->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(ControllerReference);

	SetActorEnableCollision(true);
}

void APlayerCharacter::EnableAbilities(bool _Enable)
{
	if (_Enable == false)
	{
		StopJumping();
		ActivateSprint(false);
	}
	CanLook = CanMove = CanPerformProne = CanPerformJump = CanPerformSprint = _Enable;
}

void APlayerCharacter::EnableSprint(bool _Enable)
{
	float NewSpeed = _Enable ? SprintingSpeed : NormalSpeed;
	SetSpeed(NewSpeed);

	IsSprinting = _Enable;
}

void APlayerCharacter::ActivateSprint(bool _Activate)
{
	if (IsInProne == true && _Activate == true)
	{
		ActivateProne(false, true);
	}

	EnableSprint(_Activate);
}

void APlayerCharacter::SetProne(float _VignetteIntensity, float _NewHeight) const
{
	FVector CameraLocation = Camera->GetRelativeLocation();
	CameraLocation.Z = _NewHeight;
	Camera->SetRelativeLocation(CameraLocation);

	Camera->PostProcessSettings.VignetteIntensity = _VignetteIntensity;
}

void APlayerCharacter::ActivateProne(bool _Activate, bool _ShouldAnimate)
{
	if (IsSprinting == true)
		return;

	IsInProne = _Activate;

	SetSpeed(IsInProne ? ProneSpeed : NormalSpeed);

	if (_ShouldAnimate == false)
	{
		SetProne(0, StandingHeight);
	}
	else
	{
		ProneAnimation(_Activate);
	}
}

void APlayerCharacter::SetSpeed(float _NewSpeed) const
{
	GetCharacterMovement()->MaxWalkSpeed = _NewSpeed;
}

void APlayerCharacter::SetHealth(float _NewHealth)
{
	CurrentHealth = FMath::Clamp(_NewHealth, 0.0f, MaxHealth);
	
	OnHealthChanged.Broadcast();

	if (CurrentHealth <= 0)
	{
		Die();
	}
}

void APlayerCharacter::ChangeHealth(float _HealthChange)
{
	SetHealth(CurrentHealth + _HealthChange);
}

void APlayerCharacter::SetEnergy(float _NewEnergy)
{
	CurrentEnergy = FMath::Clamp(_NewEnergy, 0.0f, MaxEnergy);
	OnEnergyChanged.Broadcast();
}

void APlayerCharacter::ChangeEnergy(float _EnergyChange)
{
	SetEnergy(CurrentEnergy + _EnergyChange);
}

void APlayerCharacter::ChangeEnergyOnDamage()
{
	ChangeEnergy(-DamageEnergyDecrease);
}

void APlayerCharacter::EnableLoadingAnimation(bool _Enable)
{
	HUDReference->SetWidgetVisible(_Enable, HUDReference->LoadingAnimationWidget);
}

void APlayerCharacter::TryTakedown()
{
	float MinDist = FLT_MAX;
	for (AAgentCharacter* Agent : AgentsInTakedownRange)
	{
		float Dist = FVector::Distance(Agent->GetActorLocation(), GetActorLocation());
		if (Dist < MinDist)
		{
			CurrentAgentInTakedownRange = Agent;
			MinDist = Dist;
		}
	}

	if (UKismetSystemLibrary::IsValid(CurrentAgentInTakedownRange) == true)
	{
		InitiateTakedown();
	}
}

void APlayerCharacter::InitiateTakedown()
{
	IsInTakedown = PerformTakedownMove = true;
	EnableAbilities(false);

	CurrentTakedownDirection = (CurrentAgentInTakedownRange->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	
	FRotator TakedownCameraRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), CurrentAgentInTakedownRange->GetActorLocation());
	ControllerReference->SetControlRotation(TakedownCameraRotation);
}

void APlayerCharacter::InitiateKill()
{
	PerformTakedownMove = false;

	SetTakedownWidgetVisible(false);
	
	if (CurrentAgentInTakedownRange->CanSeePlayer == true)
	{
		float KillAnimRate = CurrentAgentInTakedownRange->IsAttackingPlayer ? DetectedKillSlowAnimationRate : DetectedKillNormalAnimationRate;
		KillAnimation(KillAnimRate);
	}
	else
	{
		FinishTakedown();
	}
}

void APlayerCharacter::FinishTakedown()
{
	if (CurrentAgentInTakedownRange->CanSeePlayer == true && CurrentAgentInTakedownRange->IsAttackingPlayer == true)
	{
		ChangeHealth(-TakedownDetectedHealthDecrease);
	}

	CurrentAgentInTakedownRange->Death(CurrentTakedownDirection * TakedownRagdollForce);

	IsInTakedown = false;
	EnableAbilities(true);

	RemoveAgentFromTakedown(CurrentAgentInTakedownRange);
	CurrentAgentInTakedownRange = nullptr;

	SetTakedownWidgetVisible(AgentsInTakedownRange.Num() > 0);
}

void APlayerCharacter::TryConsumeBody()
{
	float MinDist = FLT_MAX;
	for (AAgentCharacter* Agent : DeadAgentsInRange)
	{
		float Dist = FVector::Distance(Agent->GetActorLocation(), GetActorLocation());
		if (Dist < MinDist)
		{
			CurrentDeadAgentInRange = Agent;
			MinDist = Dist;
		}
	}

	if (UKismetSystemLibrary::IsValid(CurrentDeadAgentInRange) == true)
	{
		InitiateConsumeBody();
	}
}

void APlayerCharacter::InitiateConsumeBody()
{
	IsEating = true;
	EnableAbilities(false);

	SetConsumeWidgetVisible(false);

	ConsumeAnimation(ConsumeAnimationRate);
}

void APlayerCharacter::FinishConsumeBody()
{
	CurrentDeadAgentInRange->Destroy();

	ChangeEnergy(ConsumeEnergyIncrease);
	ChangeHealth(ConsumeHealthIncrease);
	
	IsEating = false;
	EnableAbilities(true);

	RemoveDeadAgent(CurrentDeadAgentInRange);
	CurrentDeadAgentInRange = nullptr;

	SetConsumeWidgetVisible(DeadAgentsInRange.Num() > 0);
}

void APlayerCharacter::SwitchVision()
{
	VisionActive = !VisionActive;
	SwitchVisionAnimation(VisionActive);
}

void APlayerCharacter::TryMakeNoise()
{
	if (IsSprinting == true)
	{
		// TODO Debug This
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 0.5f, this);
	}
}

void APlayerCharacter::UpdateRaycastAndReticle()
{
	FVector Start = Camera->GetComponentLocation();
	FVector Dest = Camera->GetForwardVector() * 5000 + Camera->GetComponentLocation();

	FHitResult OutHit;
	bool HitStatus = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, Dest,
		UEngineTypes::ConvertToTraceType(HIT_COLLISION_CHANNEL), true, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true);
	if (HitStatus == true)
	{
		CurrentHitPosition = OutHit.ImpactPoint;
		AAgentCharacter* AgentCharacter = dynamic_cast<AAgentCharacter*>(OutHit.Component->GetOwner());

		if (UKismetSystemLibrary::IsValid(AgentCharacter) == true && AgentCharacter->GetIsDead() == false)
		{
			HUDReference->SetReticleRed();
		}
		else
		{
			HUDReference->SetReticleWhite();
		}
	}
	else
	{
		CurrentHitPosition = Dest;
		HUDReference->SetReticleWhite();
	}
}

void APlayerCharacter::UpdateEnergyAndHealth()
{
	float BaseEnergyDecreaseRate = VisionActive == true ? VisionActiveEnergyDecreaseRate : DefaultEnergyDecreaseRate;
	float EnergyDecreaseRate = BaseEnergyDecreaseRate + (GetIsMoving() == true ? (IsSprinting == true ? SprintEnergyDecreaseRate : MovementEnergyDecreaseRate) : 0);
	
	ChangeEnergy(-EnergyDecreaseRate * UGameplayStatics::GetWorldDeltaSeconds(this));

	if (CurrentEnergy <= 0)
	{
		if (HealthDecreaseStatus == false)
		{
			HealthDecreaseStatus = true;
			HealthDecreaseCurrentTime = 0;
		}
		
		if (HealthDecreaseCurrentTime <= 0)
		{
			ChangeHealth(-MaxHealth / HealthDecreaseSteps);
			HealthDecreaseCurrentTime = HealthDecreaseWaitTime;
		}
		HealthDecreaseCurrentTime -= UGameplayStatics::GetWorldDeltaSeconds(this);
	}
	else if (HealthDecreaseStatus == true)
	{
		HealthDecreaseStatus = false;
	}
}

void APlayerCharacter::Die()
{
	IsDead = true;

	CanPerformJump = CanPerformProne = CanPerformSprint = CanPerformTakedown = false;
	
	SetActorEnableCollision(false);

	OnPlayerDeath.Broadcast();
	
	SetTakedownWidgetVisible(false);
	SetConsumeWidgetVisible(false);
	
	HUDReference->ShouldDrawReticle = false;
	
	HUDReference->ShowGameOver();
	ControllerReference->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(ControllerReference, HUDReference->GameOverWidget);
}

void APlayerCharacter::MoveForward(float _AxisValue)
{
	if (CanMove == false)
		return;
	
	AddMovementInput(GetCameraForward(), _AxisValue);
}

void APlayerCharacter::MoveRight(float _AxisValue)
{
	if (CanMove == false)
		return;
	
	FVector CameraRight = FVector::CrossProduct(GetActorUpVector(), GetCameraForward());
	AddMovementInput(CameraRight, _AxisValue);
}

void APlayerCharacter::TurnRate(float _AxisValue)
{
	if (CanLook == false)
		return;
	
	AddControllerYawInput(_AxisValue * ControllerXSensitivity * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void APlayerCharacter::LookUpRate(float _AxisValue)
{
	if (CanLook == false)
		return;
	
	AddControllerPitchInput(_AxisValue * ControllerYSensitivity * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void APlayerCharacter::Turn(float _AxisValue)
{
	if (CanLook == false)
		return;
	
	AddControllerYawInput(_AxisValue);
}

void APlayerCharacter::LookUp(float _AxisValue)
{
	if (CanLook == false)
		return;
	
	AddControllerPitchInput(_AxisValue);
}

void APlayerCharacter::JumpPressed()
{
	if (CanPerformJump == false)
		return;
	
	Jump();
}

void APlayerCharacter::JumpReleased()
{
	StopJumping();
}

void APlayerCharacter::SprintPressed()
{
	if (CanPerformSprint == false)
		return;

	ActivateSprint(true);
}

void APlayerCharacter::SprintReleased()
{
	ActivateSprint(false);
}

void APlayerCharacter::PronePressed()
{
	if (CanPerformProne == false)
		return;

	ActivateProne(IsInProne == false, true);
}

void APlayerCharacter::TakedownPressed()
{
	if (IsInTakedown == true)
		return;

	TryTakedown();
}

void APlayerCharacter::EatPressed()
{
	if (IsEating == true)
		return;

	TryConsumeBody();
}

void APlayerCharacter::VisionPressed()
{
	if (CanSwitchVision == false)
		return;

	SwitchVision();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);

	_PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	_PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	
	_PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &APlayerCharacter::TurnRate);
	_PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);
	
	_PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &APlayerCharacter::LookUpRate);
	_PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);

	_PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &APlayerCharacter::JumpPressed);
	_PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &APlayerCharacter::JumpReleased);

	_PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::SprintPressed);
	_PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::SprintReleased);

	_PlayerInputComponent->BindAction(TEXT("Kill"), IE_Pressed, this, &APlayerCharacter::TakedownPressed);
	_PlayerInputComponent->BindAction(TEXT("Eat"), IE_Pressed, this, &APlayerCharacter::EatPressed);
	
	_PlayerInputComponent->BindAction(TEXT("Vision"), IE_Pressed, this, &APlayerCharacter::VisionPressed);
}

void APlayerCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	
	UpdateRaycastAndReticle();

	// TODO Decide what to do of this
	TryMakeNoise();

	UpdateEnergyAndHealth();

	if (PerformTakedownMove == true)
	{
		SetActorLocation(GetActorLocation() + CurrentTakedownDirection * TakedownMoveSpeed * UGameplayStatics::GetWorldDeltaSeconds(this));

		if (FVector::Distance(GetActorLocation(), CurrentAgentInTakedownRange->GetActorLocation()) < TakedownAgentOffsetDistance)
		{
			InitiateKill();
		}
	}
}

void APlayerCharacter::AddAgentForTakedown(AAgentCharacter* _Agent)
{
	AgentsInTakedownRange.Add(_Agent);
	SetTakedownWidgetVisible(AgentsInTakedownRange.Num() > 0);
}

void APlayerCharacter::RemoveAgentFromTakedown(const AAgentCharacter* _Agent)
{
	AgentsInTakedownRange.Remove(_Agent);
	SetTakedownWidgetVisible(AgentsInTakedownRange.Num() > 0);
}

void APlayerCharacter::AddDeadAgent(AAgentCharacter* _Agent)
{
	DeadAgentsInRange.Add(_Agent);
	SetConsumeWidgetVisible(DeadAgentsInRange.Num() > 0);
}

void APlayerCharacter::RemoveDeadAgent(const AAgentCharacter* _Agent)
{
	DeadAgentsInRange.Remove(_Agent);
	SetConsumeWidgetVisible(DeadAgentsInRange.Num() > 0);
}
