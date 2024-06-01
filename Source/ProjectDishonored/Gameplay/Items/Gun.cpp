// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include <stdlib.h>

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "ProjectDishonored/AI/Agent/AgentCharacter.h"
#include "ProjectDishonored/Player/PlayerCharacter.h"

//#define DEBUG_SHOOT

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot")));
	
	FireRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FireRoot"));
	FireRoot->SetupAttachment(RootComponent);
}

void AGun::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmmo = MaxAmmo;
	CurrentFiringMode = DefaultFiringMode;
	
	CanShoot = true;
}

void AGun::TryFire()
{
	bool HasAmmo = InfiniteAmmo == true || CurrentAmmo > 0;
	bool StopBurst = CurrentFiringMode == FiringMode::Burst && CurrentFireCount >= BurstFireAmount;
	
	if (HasAmmo == false || StopBurst == true)
	{
		StopShoot();
		return;
	}
	
	Fire();

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), FireRoot->GetComponentLocation(), 1.0f, this);
	
	FireFeedback();
}

void AGun::Fire()
{
	FVector StartPos = AgentOwner != nullptr ? AgentOwner->GetHeadCenterLocation() : FireRoot->GetComponentLocation();
	
	FVector ShootTargetPos = CurrentShootTarget != nullptr ? CurrentShootTarget->GetActorLocation() : CurrentShootLocation;
	FVector ShootLocalPos = ShootTargetPos - StartPos;
	
	FVector RandomShootDir =  UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ShootLocalPos.GetSafeNormal(), SpreadAngleDegrees);
	float MaxLength = std::min(ShootLocalPos.Size(), Range);
	FVector RandomShootLocalPos = RandomShootDir * MaxLength;
	FVector RandomShootPos = RandomShootLocalPos + StartPos;

	bool PlayerHit = false;
	
	FHitResult OutHit;
	bool HitStatus = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartPos, RandomShootPos,
		UEngineTypes::ConvertToTraceType(ECC_Camera), true, {AgentOwner}, EDrawDebugTrace::None, OutHit, true);
	if (HitStatus == true)
	{
		APlayerCharacter* PlayerCharacter = dynamic_cast<APlayerCharacter*>(OutHit.Component->GetOwner());
		if (UKismetSystemLibrary::IsValid(PlayerCharacter) == true && PlayerCharacter->GetIsDead() == false)
		{
			PlayerHit = true;
			PlayerCharacter->ChangeHealth(-Damage);
			PlayerCharacter->ChangeEnergyOnDamage();
		}
	}
	
	DrawDebugLine(GetWorld(), StartPos, RandomShootPos, PlayerHit ? FColor::Red : FColor::Blue, false, 2);

	FVector Up = AgentOwner->GetActorUpVector();
	FVector Right = UKismetMathLibrary::Cross_VectorVector(ShootLocalPos.GetSafeNormal(), Up);
	float SpreadAngleRadians = SpreadAngleDegrees * (PI / 180);
	DrawDebugCircle(GetWorld(), ShootTargetPos, sin(SpreadAngleRadians) * MaxLength, 32, FColor::White, false, 2, 0, 0, Right, Up);

	CurrentAmmo--;
	
	CurrentFireCount++;
}

void AGun::SetShootTarget(AActor* _Target)
{
	CurrentShootTarget = _Target;
}

void AGun::SetShootLocation(FVector _Location)
{
	CurrentShootLocation = _Location;
}

void AGun::Shoot()
{
	if (CanShoot == true)
	{
		if (CurrentFiringMode == FiringMode::Single)
		{
			TryFire();
		}
		else
		{
			IsShooting = true;
			CurrentTimeSinceLastFire = FireRate;
		}
	}
}

void AGun::StopShoot()
{
	IsShooting = false;
	CurrentFireCount = 0;
}

void AGun::Reload()
{
	// TODO
}

void AGun::ChangeFiringMode(FiringMode _NewFiringMode)
{
	CurrentFiringMode = _NewFiringMode;
}

void AGun::ResetFiringMode()
{
	CurrentFiringMode = DefaultFiringMode;
}

void AGun::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

#if defined(DEBUG_SHOOT)
  if (AgentOwner->GetIsDead() == true)
		return;

	FVector AimDir = CurrentShootTarget != nullptr ? (CurrentShootTarget->GetActorLocation() - AgentOwner->GetActorLocation()).GetSafeNormal() : AgentOwner->GetActorForwardVector();
	FVector Up = AgentOwner->GetActorUpVector();
	FVector Right = UKismetMathLibrary::Cross_VectorVector(AimDir, Up);

	float SpreadAngleRadians = SpreadAngleDegrees * (PI / 180);
	
	DrawDebugCircle(GetWorld(), AgentOwner->GetActorLocation() + AimDir * 100, sin(SpreadAngleRadians) * 100, 32, FColor::White, false, -1, 0, 0, Right, Up);
	DrawDebugCircle(GetWorld(), AgentOwner->GetActorLocation() + AimDir * 600, sin(SpreadAngleRadians) * 600, 32, FColor::Yellow, false, -1, 0, 0, Right, Up);
	DrawDebugCircle(GetWorld(), AgentOwner->GetActorLocation() + AimDir * 1000, sin(SpreadAngleRadians) * 1000, 32, FColor::Orange, false, -1, 0, 0, Right, Up);
	DrawDebugCircle(GetWorld(), AgentOwner->GetActorLocation() + AimDir * 1500, sin(SpreadAngleRadians) * 1500, 32, FColor::Red, false, -1, 0, 0, Right, Up);
#endif
	
	if (IsShooting == true)
	{
		if (CurrentTimeSinceLastFire >= FireRate)
		{
			CurrentTimeSinceLastFire = 0;
			
			TryFire();
		}
		CurrentTimeSinceLastFire += _DeltaTime;
	}
}
