// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentCharacter.h"

#include "AgentController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/AI/Navigation/Path.h"

// Sets default values
AAgentCharacter::AAgentCharacter()
{
 	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAgentCharacter::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
	PlayerReference = dynamic_cast<APlayerCharacter*>(PlayerPawn);

	ControllerReference = dynamic_cast<AAgentController*>(GetController());

	IsPathForward = Path->Forward;
	ResetPath();

	ControllerReference->RunBehavior();
	ControllerReference->Initialize();
	
	bUseControllerRotationYaw = false;
}

void AAgentCharacter::ChangeCharacterSpeed(float _NewSpeed)
{
	CharacterMovement->MaxWalkSpeed = _NewSpeed;
}

void AAgentCharacter::Stop()
{
	ControllerReference->BrainComponent->StopLogic("");
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
			Stop();
			return;
		case EPathBehavior::Loop:
			ResetPath();
			break;
		case EPathBehavior::Reverse:
			IsPathForward = !IsPathForward;
			NextInPath();
			break;
		default:
			break;
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
		CurrentPathIndex++;
	
	else CurrentPathIndex--;
}

void AAgentCharacter::ResetPath()
{
	if (IsPathForward == true)
		CurrentPathIndex = 0;
	
	else CurrentPathIndex = Path->PathPoints.Max() - 1;
}

bool AAgentCharacter::CheckPathEnd()
{
	if (IsPathForward == true)
		return CurrentPathIndex >= Path->PathPoints.Max();
	
	return CurrentPathIndex < 0;
}

void AAgentCharacter::UpdatePlayerKillStatus(bool _CanTakedown)
{
	bool CanActuallyTakedown = _CanTakedown && PlayerReference->GetCanPerformTakedown();
	PlayerReference->CurrentAgentInRange = CanActuallyTakedown ? this : nullptr;
	SetTakedownUIVisible(CanActuallyTakedown);
}

void AAgentCharacter::CheckKillDistance()
{
	if (PlayerReference == nullptr || PlayerReference->GetIsInTakedown() == true)
		return;

	bool InsideRange = FVector::Distance(GetActorLocation(), PlayerReference->GetActorLocation()) <= TakedownRadius;

	bool InsideHeight = UKismetMathLibrary::Abs(PlayerReference->GetActorLocation().Z - GetActorLocation().Z) <= TakedownHeightRange;

	if ((InsideRange && InsideHeight) != CanTakedown)
	{
		CanTakedown = InsideRange && InsideHeight;
		UpdatePlayerKillStatus(CanTakedown);
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
		Projectile->Destroy();
}

// Called every frame
void AAgentCharacter::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (IsDead == false)
		CheckKillDistance();
}

// Called to bind functionality to input
void AAgentCharacter::SetupPlayerInputComponent(UInputComponent* _PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(_PlayerInputComponent);

}

void AAgentCharacter::EnableCharacter(bool _Enable)
{
	SetActorEnableCollision(_Enable);
	
	Mesh->bPauseAnims = _Enable == false;

	if (_Enable == false)
		Stop();
}

void AAgentCharacter::Death(FVector _HitDirection)
{
	IsDead = true;

	CharacterMovement->DisableMovement();
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Mesh->SetSimulatePhysics(true);
	// TODO Make this bone name a variable you have to fill in as a user
	Mesh->AddImpulse(_HitDirection, "head");

	SetTakedownUIVisible(false);

	ControllerReference->SetDetectionVisibility(false);
	// TODO Find a solution to make the name available for editing by the user
	ControllerReference->GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("DeadState"), true);
}
