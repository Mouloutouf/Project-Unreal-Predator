// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/AI/Agent/AgentCharacter.h"
#include "ProjectDishonored/Player/PlayerCharacter.h"

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
	if (CurrentAmmo <= 0 || (CurrentFiringMode == FiringMode::Burst && CurrentFireCount > BurstFireAmount))
	{
		StopShoot();
		return;
	}
	
	Fire();

	FireFeedback();
}

void AGun::Fire()
{
	FVector ShootLocation = CurrentShootTarget != nullptr ? CurrentShootTarget->GetActorLocation() : CurrentShootLocation;
	FVector StartLocation = AgentOwner != nullptr ? AgentOwner->GetHeadCenterLocation() : FireRoot->GetComponentLocation();
	
	FHitResult OutHit;
	bool HitStatus = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocation, ShootLocation,
		UEngineTypes::ConvertToTraceType(ECC_Camera), true, {AgentOwner}, EDrawDebugTrace::None, OutHit, true);
	if (HitStatus == true)
	{
		APlayerCharacter* PlayerCharacter = dynamic_cast<APlayerCharacter*>(OutHit.Component->GetOwner());
		if (UKismetSystemLibrary::IsValid(PlayerCharacter) == true && PlayerCharacter->GetIsDead() == false)
		{
			PlayerCharacter->ChangeHealth(-Damage);
		}
	}

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
