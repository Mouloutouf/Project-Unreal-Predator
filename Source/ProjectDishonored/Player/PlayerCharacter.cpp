// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "ProjectDishonored/ProjectDishonored.h"
#include "ProjectDishonored/AI/Agent/AgentCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	TakedownRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TakedownRoot"));
	TakedownRoot->SetupAttachment(RootComponent);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CanPerformJump = CanPerformCrouch = CanPerformSprint = CanPerformTakedown = true;

	ControllerReference = dynamic_cast<APlayerController*>(GetController());
	
	HUDReference = dynamic_cast<APlayerHUD*>(ControllerReference->GetHUD());
	HUDReference->ShouldDrawReticle = true;

	StandingHeight = Camera->GetRelativeLocation().Z;
	Camera->PostProcessSettings.VignetteIntensity = DefaultVignetteIntensity;
	
	OnHealthChanged.AddDynamic(this, &APlayerCharacter::UpdateHealthUI);
	OnEnergyChanged.AddDynamic(this, &APlayerCharacter::UpdateEnergyUI);

	SetHealth(MaxHealth);
	SetEnergy(MaxEnergy);

	NormalSpeed = GetCharacterMovement()->MaxWalkSpeed;

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
	CanLook = CanMove = CanPerformCrouch = CanPerformJump = CanPerformSprint = _Enable;
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
		ActivateProne(false, true);

	EnableSprint(_Activate);
}

void APlayerCharacter::SetProne(float _VignetteIntensity, float _NewHeight)
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
		SetProne(0, StandingHeight);
	else
		CrouchAnimation(_Activate);
}

void APlayerCharacter::SetSpeed(float _NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = _NewSpeed;
}

void APlayerCharacter::SetHealth(float _NewHealth)
{
	CurrentHealth = FMath::Clamp(_NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast();
}

void APlayerCharacter::ChangeHealth(float _HealthChange)
{
	SetHealth(CurrentHealth + _HealthChange);

	if (CurrentHealth <= 0)
		Die();
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

void APlayerCharacter::EnableLoadingAnimation(bool _Enable)
{
	HUDReference->SetWidgetVisible(_Enable, HUDReference->LoadingAnimationWidget);
}

void APlayerCharacter::InitiateTakedown()
{
	IsInTakedown = PerformTakedownMove = true;
	EnableAbilities(false);

	CurrentTakedownDirection = (CurrentAgentInKillRange->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	
	FRotator TakedownCameraRotation = UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(), CurrentAgentInKillRange->GetActorLocation());
	ControllerReference->SetControlRotation(TakedownCameraRotation);
}

void APlayerCharacter::TakedownKill()
{
	PerformTakedownMove = false;

	SetTakedownWidgetVisible(false);
	
	if (CurrentAgentInKillRange->IsPlayerDetected == true)
	{
		float KillAnimRate = CurrentAgentInKillRange->CurrentSuspicionLevel == 2 ? DetectedKillSlowAnimationRate : DetectedKillNormalAnimationRate;
		KillAnimation(KillAnimRate);
	}
	else
		FinishTakedown();
}

void APlayerCharacter::FinishTakedown()
{
	CurrentAgentInKillRange->Death(FVector::ZeroVector);
	
	if (CurrentAgentInKillRange->CurrentSuspicionLevel == 2)
		ChangeHealth(-TakedownDetectedHealthDecrease);

	IsInTakedown = false;
	EnableAbilities(true);

	CurrentAgentInKillRange = nullptr;
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
	
	IsEating = false;
	EnableAbilities(true);

	CurrentDeadAgentInRange = nullptr;
}

void APlayerCharacter::TryMakeNoise()
{
	if (IsSprinting == true)
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 0.5f, this);
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
			HUDReference->SetReticleRed();
		else
			HUDReference->SetReticleWhite();
	}
	else
	{
		CurrentHitPosition = Dest;
		HUDReference->SetReticleWhite();
	}
}

void APlayerCharacter::UpdateIsMovingForward(bool _State)
{
	if (_State == IsMovingForward)
		return;

	IsMovingForward = _State;

	if (IsSprinting == true && IsMovingForward == false)
		ActivateSprint(false);
}

void APlayerCharacter::DecreaseEnergy()
{
	float EnergyDecreaseSpeed = GetIsMoving() == true ? EnergyMovementDecreaseSpeed : EnergyImmobileDecreaseSpeed;
	EnergyDecreaseSpeed *= IsSprinting == true ? EnergySprintDecreaseFactor : 1;
	
	ChangeEnergy(-EnergyDecreaseSpeed * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void APlayerCharacter::DecreaseHealth()
{
	ChangeHealth(-MaxHealth / HealthDecreaseSteps);
}

void APlayerCharacter::CheckEnergy()
{
	if (CurrentEnergy <= 0 && ShouldDecreaseHealth == false)
	{
		ShouldDecreaseHealth = true;
		HealthDecreaseCurrentTime = HealthDecreaseWaitTime;
	}
	
	if (CurrentEnergy > 0 && ShouldDecreaseHealth == true)
		ShouldDecreaseHealth = false;
}

void APlayerCharacter::TryDecreaseHealth()
{
	if (ShouldDecreaseHealth == true)
	{
		if (HealthDecreaseCurrentTime >= HealthDecreaseWaitTime)
		{
			DecreaseHealth();
			HealthDecreaseCurrentTime = 0;
		}
		HealthDecreaseCurrentTime += UGameplayStatics::GetWorldDeltaSeconds(this);
	}
}

void APlayerCharacter::Die()
{
	IsDead = true;

	CanPerformJump = CanPerformCrouch = CanPerformSprint = CanPerformTakedown = false;
	
	SetActorEnableCollision(false);

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

	UpdateIsMovingForward(_AxisValue > 0.0f);
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
	if (CanPerformJump)
		Jump();
}

void APlayerCharacter::JumpReleased()
{
	StopJumping();
}

void APlayerCharacter::SprintPressed()
{
	if (CanPerformSprint == true && IsMovingForward == true)
		ActivateSprint(true);
}

void APlayerCharacter::SprintReleased()
{
	ActivateSprint(false);
}

void APlayerCharacter::PronePressed()
{
	if (CanPerformCrouch == true)
		ActivateProne(IsInProne == false, true);
}

void APlayerCharacter::TakedownPressed()
{
	if (UKismetSystemLibrary::IsValid(CurrentAgentInKillRange) == false || IsInTakedown == true)
		return;

	InitiateTakedown();
}

void APlayerCharacter::EatPressed()
{
	if (UKismetSystemLibrary::IsValid(CurrentDeadAgentInRange) == false || IsEating == true)
		return;

	InitiateConsumeBody();
}

// Called every frame
void APlayerCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
	
	TryMakeNoise();
	UpdateRaycastAndReticle();

	DecreaseEnergy();
	CheckEnergy();
	TryDecreaseHealth();

	if (PerformTakedownMove == true)
	{
		SetActorLocation(GetActorLocation() + CurrentTakedownDirection * TakedownMoveSpeed * UGameplayStatics::GetWorldDeltaSeconds(this));

		if (FVector::Distance(GetActorLocation(), CurrentAgentInKillRange->GetActorLocation()) < TakedownAgentOffsetDistance)
			TakedownKill();
	}
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
}

