// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/ProjectDishonored.h"
#include "ProjectDishonored/Player/PlayerCharacter.h"


AGun::AGun()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot")));
	
	FireRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FireRoot"));
	FireRoot->SetupAttachment(RootComponent);
}

void AGun::TryFire()
{
	if (CurrentAmmo <= 0 || (CurrentFiringMode == FiringMode::Burst && CurrentFireCount > BurstFireAmount))
	{
		StopShoot();
		return;
	}
	
	Fire();

	// TODO Fire Feedback
}

void AGun::Fire()
{
	FVector ShootLocation = CurrentShootTarget != nullptr ? CurrentShootTarget->GetActorLocation() : CurrentShootLocation;
	
	FHitResult OutHit;
	bool HitStatus = UKismetSystemLibrary::LineTraceSingle(GetWorld(), FireRoot->GetComponentLocation(), ShootLocation,
		UEngineTypes::ConvertToTraceType(HIT_COLLISION_CHANNEL), true, TArray<AActor*>(), EDrawDebugTrace::None, OutHit, true);
	if (HitStatus == true)
	{
		APlayerCharacter* PlayerCharacter = dynamic_cast<APlayerCharacter*>(OutHit.Component->GetOwner());
		if (UKismetSystemLibrary::IsValid(PlayerCharacter) == true && PlayerCharacter->GetIsDead() == false)
		{
			// Damage Player
		}
	}

	CurrentAmmo--;
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
	//
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
			CurrentFireCount++;
			
			TryFire();
		}
		CurrentTimeSinceLastFire += _DeltaTime;
	}
}
