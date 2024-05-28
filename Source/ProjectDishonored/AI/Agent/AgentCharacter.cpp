// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentCharacter.h"

#include "AgentController.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/ProjectDishonored.h"
#include "ProjectDishonored/AI/Navigation/Path.h"
#include "ProjectDishonored/Gameplay/Items/Gun.h"
#include "ProjectDishonored/Player/Projectile.h"

AAgentCharacter::AAgentCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	VoiceComponent = CreateDefaultSubobject<UAgentVoice>(TEXT("Agent Voice"));
}

void AAgentCharacter::BeginPlay()
{
	Super::BeginPlay();

	bUseControllerRotationYaw = false;

	CustomDepthStencilAlive = Mesh->CustomDepthStencilValue;
	
	GatherDetectableCapsules();

	TrySetAgentWeapon();
	if (Weapon != nullptr)
	{
		Weapon->SetWeaponOwner(this);
	}
	
	if (UKismetSystemLibrary::IsValid(Path) == true)
	{
		IsPathForward = Path->Forward;
		ResetPath();
	}
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
	PlayerReference = dynamic_cast<APlayerCharacter*>(PlayerPawn);

	ControllerReference = dynamic_cast<AAgentController*>(GetController());

	ControllerReference->InitBehavior();
}

// TODO Make better use of this
void AAgentCharacter::Stop()
{
	ControllerReference->Stop();
}

void AAgentCharacter::SetNextDestination()
{
	if (UKismetSystemLibrary::IsValid(Path) == false)
		return;

	if (CheckPathEnd() == true)
	{
		switch (Path->PathBehavior)
		{
		case EPathBehavior::Stop:
			{
				Stop();
				return;
			}
		case EPathBehavior::Loop:
			{
				ResetPath();
				break;
			}
		case EPathBehavior::Reverse:
			{
				IsPathForward = !IsPathForward;
				NextInPath();
				break;
			}
		default:
			{
				break;
			}
		}
	}

	SetDestinationInfo(Path->PathPoints[CurrentPathIndex]);
	AtDestination = false;
	NextInPath();
}

void AAgentCharacter::SetDestinationInfo(FPathPointInfo _PathPointInfo)
{
	CurrentPoint = _PathPointInfo.PathPoint;
	CurrentPoint->WaitTime = _PathPointInfo.WaitTime;
}

void AAgentCharacter::NextInPath()
{
	if (IsPathForward == true)
	{
		CurrentPathIndex++;
	}
	else
	{
		CurrentPathIndex--;
	}
}

void AAgentCharacter::ResetPath()
{
	if (IsPathForward == true)
	{
		CurrentPathIndex = 0;
	}
	else
	{
		CurrentPathIndex = Path->PathPoints.Max() - 1;
	}
}

bool AAgentCharacter::CheckPathEnd() const
{
	if (IsPathForward == true)
		return CurrentPathIndex >= Path->PathPoints.Max();
	
	return CurrentPathIndex < 0;
}

void AAgentCharacter::CheckPlayerCanTakedown()
{
	if (PlayerReference == nullptr || PlayerReference->GetIsInTakedown() == true)
		return;

	float KillRadius = ControllerReference->GetPlayerVisible() == true ? PlayerReference->DetectedKillRadius : PlayerReference->UndetectedKillRadius;
	bool InsideRange = FVector::Distance(GetBodyCenterLocation(), PlayerReference->GetActorLocation()) <= KillRadius;

	bool InsideHeight = UKismetMathLibrary::Abs(PlayerReference->GetActorLocation().Z - GetBodyCenterLocation().Z) <= PlayerReference->KillHeightRange;

	bool HasLineOfTakedown = false;
	if (InsideRange && InsideHeight)
	{
		FHitResult HitResult;
		bool Hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetBodyCenterLocation(), PlayerReference->GetActorLocation(),
			UEngineTypes::ConvertToTraceType(HIT_COLLISION_CHANNEL), true, TArray<AActor*>({ this }), EDrawDebugTrace::None, HitResult, true);
		if (Hit == true)
		{
			HasLineOfTakedown = HitResult.Component->GetOwner() == PlayerReference;
		}
		
		DrawDebugLine(GetWorld(), PlayerReference->GetActorLocation(), GetBodyCenterLocation(), HasLineOfTakedown ? FColor::Green : FColor::Red);
	}
	
	if (HasLineOfTakedown != CanPlayerTakedown)
	{
		CanPlayerTakedown = HasLineOfTakedown;
		UpdatePlayerCanTakedown(CanPlayerTakedown);
	}
}

void AAgentCharacter::UpdatePlayerCanTakedown(bool _CanTakedown)
{
	bool CanPerformTakedown = _CanTakedown && PlayerReference->GetCanPerformTakedown();
	
	if (CanPerformTakedown)
	{
		PlayerReference->AddAgentForTakedown(this);
	}
	else
	{
		PlayerReference->RemoveAgentFromTakedown(this);
	}
}

// TODO Add a check for if the player is actually looking in the direction of the body
void AAgentCharacter::CheckPlayerCanConsume()
{
	if (PlayerReference == nullptr || PlayerReference->GetIsEating() == true)
		return;

	bool InsideRange = FVector::Distance(GetBodyCenterLocation(), PlayerReference->GetActorLocation()) <= PlayerReference->ConsumeRadius;
	bool InsideHeight = UKismetMathLibrary::Abs(PlayerReference->GetActorLocation().Z - GetBodyCenterLocation().Z) <= PlayerReference->ConsumeHeightRange;

	if ((InsideRange && InsideHeight) != CanPlayerConsume)
	{
		CanPlayerConsume = InsideRange && InsideHeight;
		UpdatePlayerCanConsume(CanPlayerConsume);
	}
}

// TODO Should create some sort of generic interaction component which allows an object to expose an interaction to the player
void AAgentCharacter::UpdatePlayerCanConsume(bool _CanConsume)
{
	if (_CanConsume)
	{
		PlayerReference->AddDeadAgent(this);
	}
	else
	{
		PlayerReference->RemoveDeadAgent(this);
	}
}

void AAgentCharacter::StopShooting() const
{
	AGun* AgentGun = dynamic_cast<AGun*>(Weapon);
	if (AgentGun != nullptr)
	{
		AgentGun->CanShoot = false;
		AgentGun->StopShoot();
	}
}

void AAgentCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (PlayerReference->GetIsDead() == true)
		return;
	
	if (IsDead == false)
	{
		CheckPlayerCanTakedown();
	}
	else
	{
		CheckPlayerCanConsume();
	}
}

void AAgentCharacter::ChangeCharacterSpeed(float _NewSpeed) const
{
	CharacterMovement->MaxWalkSpeed = _NewSpeed;
}

void AAgentCharacter::EnableCharacter(bool _Enable)
{
	SetActorEnableCollision(_Enable);
	
	Mesh->bPauseAnims = _Enable == false;

	if (_Enable == false)
	{
		Stop();
	}
}

void AAgentCharacter::TryDeathByProjectile(AActor* _Other)
{
	if (IsDead == true)
		return;

	AProjectile* Projectile = dynamic_cast<AProjectile*>(_Other);
	if (UKismetSystemLibrary::IsValid(Projectile) == false)
		return;

	if (Projectile->GetIsMoving() == false)
		return;

	FVector HitDirection = Projectile->GetRootComponent()->GetComponentRotation().Vector() * HitForce;
	Death(HitDirection);

	if (UKismetSystemLibrary::IsValid(Projectile) == true)
	{
		Projectile->Destroy();
	}
}

void AAgentCharacter::Death(FVector _HitDirection)
{
	IsDead = true;

	StopShooting();

	VoiceComponent->StopCurrentVoiceline();
	
	CharacterMovement->DisableMovement();
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Mesh->SetSimulatePhysics(true);
	for (FName BoneToHit : BonesToHitOnDeath)
	{
		Mesh->AddImpulse(_HitDirection, BoneToHit);
	}

	Mesh->SetCustomDepthStencilValue(CustomDepthStencilDead);

	PlayerReference->RemoveAgentFromTakedown(this);

	ControllerReference->SetDetectionVisibility(false);
	ControllerReference->SetDeathStatus();
}
