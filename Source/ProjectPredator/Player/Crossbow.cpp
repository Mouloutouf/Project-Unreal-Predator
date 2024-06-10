// Fill out your copyright notice in the Description page of Project Settings.

#include "Crossbow.h"

#include "Kismet/KismetSystemLibrary.h"

ACrossbow::ACrossbow()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(Root);

	ProjectileRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileRoot"));
	ProjectileRoot->SetupAttachment(RootComponent);
}

void ACrossbow::BeginPlay()
{
	Super::BeginPlay();

	LoadProjectile();
	OnAmmoUpdated.AddDynamic(this, &ACrossbow::TryReload);

	CanShoot = true;

	CurrentAmmo = StartAmmo;
	UpdateAmmoUI();
	OnAmmoUpdated.AddDynamic(this, &ACrossbow::UpdateAmmoUI);
}

void ACrossbow::LoadProjectile()
{
	if (UKismetSystemLibrary::IsValid(LoadedProjectile) == true)
		return;

	CreateProjectile();

	if (UKismetSystemLibrary::IsValid(LoadedProjectile) == true)
	{
		EAttachmentRule AttachmentRule = EAttachmentRule::KeepWorld;
		FAttachmentTransformRules&& LoadedProjectileAttachmentRules = FAttachmentTransformRules(AttachmentRule, AttachmentRule, AttachmentRule, true);
		LoadedProjectile->AttachToActor(this, LoadedProjectileAttachmentRules);
	}
}

void ACrossbow::TryReload()
{
	if (CurrentAmmo > 0 && UKismetSystemLibrary::IsValid(LoadedProjectile) == false)
	{
		StartReload();
	}
}

void ACrossbow::StartReload()
{
	CanShoot = false;
	ReloadFirstPart();
}

void ACrossbow::ReloadProjectile()
{
	LoadProjectile();
	ReloadSecondPart();
}

void ACrossbow::FinishReload()
{
	CanShoot = true;
}

void ACrossbow::ShootProjectile(FVector _ClosestHitPosition)
{
	EDetachmentRule DetachmentRule = EDetachmentRule::KeepWorld;
	FDetachmentTransformRules&& LoadedProjectileDetachmentRules = FDetachmentTransformRules(DetachmentRule, DetachmentRule, DetachmentRule, true);
	LoadedProjectile->DetachFromActor(LoadedProjectileDetachmentRules);

	FVector Direction = (_ClosestHitPosition - ProjectileRoot->GetComponentLocation()).GetSafeNormal();
	FRotator NewRotation = Direction.ToOrientationRotator();
	LoadedProjectile->GetRootComponent()->SetWorldRotation(NewRotation);

	LoadedProjectile->LaunchProjectile();
	LoadedProjectile = nullptr;
}

void ACrossbow::TryShootProjectile(FVector _ClosestHitPosition)
{
	if (UKismetSystemLibrary::IsValid(LoadedProjectile) == false || CanShoot == false)
		return;

	ShootProjectile(_ClosestHitPosition);

	DecrementAmmo();
}

void ACrossbow::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}

void ACrossbow::IncrementAmmo()
{
	CurrentAmmo++;
	OnAmmoUpdated.Broadcast();
}

void ACrossbow::DecrementAmmo()
{
	CurrentAmmo--;
	OnAmmoUpdated.Broadcast();
}