// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"

#include "Components/CapsuleComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(Root);
	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (IsMoving == true)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AProjectile::DestroyProjectile, 10.0f, false);
	}
}

void AProjectile::LaunchProjectile()
{
	FVector&& Velocity = FVector(ProjectileMovement->InitialSpeed, 0, 0);
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);

	ProjectileMovement->Activate();
	IsMoving = true;
}

void AProjectile::DestroyProjectile()
{
	Destroy();
}
