// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentCharacter.h"

#include "AgentController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/ProjectDishonored.h"
#include "ProjectDishonored/AI/Navigation/Path.h"
#include "ProjectDishonored/Player/Projectile.h"

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
	ControllerReference->PlayerReference = PlayerReference;

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

// TODO Should make sure it is always the closest agent to the player that is selected, if there are multiple ones, so they don't overwrite each other constantly
void AAgentCharacter::UpdatePlayerCanTakedown(bool _CanTakedown)
{
	bool CanPerformTakedown = _CanTakedown && PlayerReference->GetCanPerformTakedown();
	PlayerReference->CurrentAgentInKillRange = CanPerformTakedown ? this : nullptr;
	PlayerReference->SetTakedownWidgetVisible(CanPerformTakedown);
}

void AAgentCharacter::CheckPlayerCanTakedown()
{
	if (PlayerReference == nullptr || PlayerReference->GetIsInTakedown() == true)
		return;

	float KillRadius = ControllerReference->GetPlayerDetected() == true ? PlayerReference->DetectedKillRadius : PlayerReference->UndetectedKillRadius;
	bool InsideRange = FVector::Distance(GetActorLocation(), PlayerReference->GetActorLocation()) <= KillRadius;

	bool InsideHeight = UKismetMathLibrary::Abs(PlayerReference->GetActorLocation().Z - GetActorLocation().Z) <= PlayerReference->KillHeightRange;

	bool HasLineOfTakedown = false;
	if (InsideRange && InsideHeight)
	{
		FHitResult HitResult;
		bool Hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(), PlayerReference->GetActorLocation(),
			UEngineTypes::ConvertToTraceType(HIT_COLLISION_CHANNEL), true, TArray<AActor*>({ this }), EDrawDebugTrace::None, HitResult, true);
		if (Hit == true)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("HIT OBJECT %s"), *HitResult.Component->GetOwner()->GetName()));
			HasLineOfTakedown = HitResult.Component->GetOwner() == PlayerReference;
		}
		
		DrawDebugLine(GetWorld(), PlayerReference->GetActorLocation(), GetActorLocation(), HasLineOfTakedown ? FColor::Green : FColor::Red);
	}
	
	if (HasLineOfTakedown != CanPlayerTakedown)
	{
		CanPlayerTakedown = HasLineOfTakedown;
		UpdatePlayerCanTakedown(CanPlayerTakedown);
	}
}

// TODO Should create some sort of generic interaction component which allows an object to expose an interaction to the player
void AAgentCharacter::UpdatePlayerCanConsume(bool _CanConsume)
{
	PlayerReference->CurrentDeadAgentInRange = _CanConsume ? this : nullptr;
	PlayerReference->SetConsumeWidgetVisible(_CanConsume);
}

// TODO Add a check for if the player is actually looking in the direction of the body
void AAgentCharacter::CheckPlayerCanConsume()
{
	if (PlayerReference == nullptr || PlayerReference->GetIsEating() == true)
		return;

	bool InsideRange = FVector::Distance(Mesh->GetBoneLocation("pelvis"), PlayerReference->GetActorLocation()) <= PlayerReference->ConsumeRadius;

	bool InsideHeight = UKismetMathLibrary::Abs(PlayerReference->GetActorLocation().Z - Mesh->GetBoneLocation("pelvis").Z) <= PlayerReference->ConsumeHeightRange;

	if ((InsideRange && InsideHeight) != CanPlayerConsume)
	{
		CanPlayerConsume = InsideRange && InsideHeight;
		UpdatePlayerCanConsume(CanPlayerConsume);
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
		CheckPlayerCanTakedown();
	else
		CheckPlayerCanConsume();
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

	if (PlayerReference != nullptr)
		PlayerReference->SetTakedownWidgetVisible(false);

	ControllerReference->SetDetectionVisibility(false);
	// TODO Find a solution to make the name available for editing by the user
	ControllerReference->GetBlackboardComponent()->SetValueAsBool(UKismetSystemLibrary::MakeLiteralName("DeadState"), true);
}
