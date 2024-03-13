// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "ProjectDishonored/AI/Agent/AgentCharacter.h"

#define HIT_COLLISION_CHANNEL ECC_GameTraceChannel1
#define PROJECTILE_COLLISION_CHANNEL ECC_GameTraceChannel2

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	
	TakedownRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TakedownRoot"));
	TakedownRoot->SetupAttachment(RootComponent);

	ChildCrossbow = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildCrossbow"));
	ChildCrossbow->SetupAttachment(FirstPersonCamera);
	ChildCrossbow->SetChildActorClass(ACrossbow::StaticClass());
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

	CrossbowReference = dynamic_cast<ACrossbow*>(ChildCrossbow->GetChildActor());
	
	ControllerReference = dynamic_cast<APlayerController*>(GetController());
	
	HUDReference = dynamic_cast<APlayerHUD*>(ControllerReference->GetHUD());
	HUDReference->ShouldDrawReticle = true;

	StandingHeight = FirstPersonCamera->GetRelativeLocation().Z;
	FirstPersonCamera->PostProcessSettings.VignetteIntensity = DefaultVignetteIntensity;
	
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
	CrossbowReference->GetRootComponent()->SetVisibility(_Enable, true);
	CrossbowReference->CanShoot = _Enable;

	if (_Enable == false)
	{
		ActivateCrouch(false, false);
		StopJumping();
		ActivateSprint(false);
	}
	CanPerformCrouch = CanPerformJump = CanPerformSprint = _Enable;
}

void APlayerCharacter::EnableSprint(bool _Enable)
{
	float NewSpeed = _Enable ? SprintingSpeed : NormalSpeed;
	SetSpeed(NewSpeed);

	IsSprinting = _Enable;
}

void APlayerCharacter::ActivateSprint(bool _Activate)
{
	if (IsCrouching == true && _Activate == true)
		ActivateCrouch(false, true);

	EnableSprint(_Activate);
}

void APlayerCharacter::SetCrouch(float _VignetteIntensity, float _NewHeight)
{
	FVector CameraLocation = FirstPersonCamera->GetRelativeLocation();
	CameraLocation.Z = _NewHeight;
	FirstPersonCamera->SetRelativeLocation(CameraLocation);

	FirstPersonCamera->PostProcessSettings.VignetteIntensity = _VignetteIntensity;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("SET VIGNETTE TO %f"), _VignetteIntensity));
}

void APlayerCharacter::ActivateCrouch(bool _Activate, bool _ShouldAnimate)
{
	if (IsSprinting == true)
		return;

	IsCrouching = _Activate;

	SetSpeed(IsCrouching ? CrouchingSpeed : NormalSpeed);

	if (_ShouldAnimate == false)
		SetCrouch(0, StandingHeight);
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

// DEPRECATED
void APlayerCharacter::EnableTakedownUI(bool _Enable)
{
	HUDReference->SetWidgetVisible(_Enable, HUDReference->TakedownLoadWidget);
}

void APlayerCharacter::InitiateTakedown()
{
	IsInTakeDown = true;
	EnableAbilities(false);

	CurrentTakedownDirection = (CurrentAgentInRange->GetActorLocation() - GetActorLocation()).GetSafeNormal();
}

void APlayerCharacter::FinishTakedown()
{
	CurrentAgentInRange->Death(FVector::ZeroVector);
	CurrentAgentInRange = nullptr;

	EnableAbilities(true);
	IsInTakeDown = false;
}

void APlayerCharacter::TryMakeNoise()
{
	if (IsSprinting == true)
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 0.5f, this);
}

void APlayerCharacter::UpdateRaycastAndReticle()
{
	FVector Start = FirstPersonCamera->GetComponentLocation();
	FVector Dest = FirstPersonCamera->GetForwardVector() * 5000 + FirstPersonCamera->GetComponentLocation();

	FHitResult OutHit;
	bool HitStatus = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, Dest, UEngineTypes::ConvertToTraceType(HIT_COLLISION_CHANNEL), true, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true);
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
	AddMovementInput(GetActorForwardVector(), _AxisValue);

	UpdateIsMovingForward(_AxisValue > 0.0f);
}

void APlayerCharacter::MoveRight(float _AxisValue)
{
	AddMovementInput(GetActorRightVector(), _AxisValue);
}

void APlayerCharacter::TurnRate(float _AxisValue)
{
	AddControllerYawInput(_AxisValue * ControllerXSensitivity * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void APlayerCharacter::LookUpRate(float _AxisValue)
{
	AddControllerPitchInput(_AxisValue * ControllerYSensitivity * UGameplayStatics::GetWorldDeltaSeconds(this));
}

void APlayerCharacter::Turn(float _AxisValue)
{
	AddControllerYawInput(_AxisValue);
}

void APlayerCharacter::LookUp(float _AxisValue)
{
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

void APlayerCharacter::ShootPressed()
{
	CrossbowReference->TryShootProjectile(CurrentHitPosition);
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

void APlayerCharacter::CrouchPressed()
{
	if (CanPerformCrouch == true)
		ActivateCrouch(IsCrouching == false, true);
}

void APlayerCharacter::TakedownPressed()
{
	if (UKismetSystemLibrary::IsValid(CurrentAgentInRange) == false || IsInTakeDown == true)
		return;

	InitiateTakedown();
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

	if (IsInTakeDown == true)
	{
		SetActorLocation(GetActorLocation() + CurrentTakedownDirection * TakedownSpeed * UGameplayStatics::GetWorldDeltaSeconds(this));

		if (FVector::Distance(GetActorLocation(), CurrentAgentInRange->GetActorLocation()) < TakedownAgentOffsetDistance)
			FinishTakedown();
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

	_PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &APlayerCharacter::ShootPressed);

	_PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::SprintPressed);
	_PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::SprintReleased);

	_PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &APlayerCharacter::CrouchPressed);

	_PlayerInputComponent->BindAction(TEXT("Kill"), IE_Pressed, this, &APlayerCharacter::TakedownPressed);
}

