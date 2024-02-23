// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(Root);
	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (IsMoving == true)
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AProjectile::DestroyProjectile, 10.0f, false);
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

