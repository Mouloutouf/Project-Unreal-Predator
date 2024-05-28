// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentController.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/Utility/MathUtility.h"

void AAgentController::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
	PlayerReference = dynamic_cast<APlayerCharacter*>(PlayerPawn);

	PlayerReference->OnPlayerDeath.AddDynamic(this, &AAgentController::OnPlayerDeath);
	
	ControlledAgent = dynamic_cast<AAgentCharacter*>(GetPawn());

	ChangeSuspicion(0);
	
	SetTimelinePlayRate(1 / MinSuspicionIncreaseRate);
}

void AAgentController::Stop() const
{
	BrainComponent->StopLogic("");
}

void AAgentController::SetDeathStatus() const
{
	Blackboard->SetValueAsBool("DeadState", true);
}

void AAgentController::UpdateDetectionMeterAngle() const
{
	if (UKismetSystemLibrary::IsValid(DetectionMeterWidget) == false)
		return;
	
	FVector AgentPositionFromPlayer = ControlledAgent->GetActorLocation() - PlayerReference->GetActorLocation();
	FVector PlayerDirection = PlayerReference->GetCameraForward();
	
	float Angle = -MathUtility::AngleBetweenVectors(AgentPositionFromPlayer, PlayerDirection);
	DetectionMeterWidget->SetRenderTransformAngle(Angle);
}

float AAgentController::GetDetectionIncreaseRate()
{
	float MaxIncreaseRate = SuspicionLevel == 0 ? MaxSuspicionIncreaseRate : MaxLureIncreaseRate;
	float MinIncreaseRate = SuspicionLevel == 0 ? MinSuspicionIncreaseRate : MinLureIncreaseRate;
	
	float DistanceToPlayer = FVector::Distance(ControlledAgent->GetActorLocation(), PlayerReference->GetActorLocation());
	float DistanceAlpha = DistanceToPlayer / IncreaseRateDistanceRatio;
	
	float DistanceToPlayerDetectionRate = UKismetMathLibrary::Lerp(MaxIncreaseRate, MinIncreaseRate, DistanceAlpha);
	
	float PlayerSpeedDetectionRate = PlayerReference->GetIsInProne() ? PlayerInProneDetectionRate : PlayerReference->GetIsSprinting() ? PlayerSprintingDetectionRate : PlayerNormalDetectionRate;
	
	return 1 / (DistanceToPlayerDetectionRate * PlayerSpeedDetectionRate);
}

float AAgentController::GetDetectionDecreaseRate()
{
	float DecreaseRate = SuspicionLevel == 0 ? SuspicionDecreaseRate : SuspicionLevel == 1 ? LureDecreaseRate : ChaseDecreaseRate;
	return 1 / DecreaseRate;
}

void AAgentController::IncreaseSuspicion()
{
	ChangeSuspicion(SuspicionLevel + 1);
}

void AAgentController::DecreaseSuspicion()
{
	ChangeSuspicion(SuspicionLevel - 1);
}

void AAgentController::ChangeSuspicion(int _Value)
{
	if (_Value < 0 || _Value > 2)
		return;

	SuspicionLevel = _Value;
	
	ControlledAgent->IsAttackingPlayer = SuspicionLevel == 2;

	OnSuspicionChanged();
}

void AAgentController::OnSuspicionChanged()
{
	SetAimedStatus(SuspicionLevel > 0);
	
	UpdateDisplayedSuspicionUI();

	float Speed = SuspicionLevel == 0 ? PatrolSpeed : SuspicionLevel == 1 ? LureSpeed : ChaseSpeed;
	ControlledAgent->ChangeCharacterSpeed(Speed);
}

bool AAgentController::TrySetPlayerAsLurePosition()
{
	if (SuspicionLevel != 1)
		return false;

	if (PlayerVisible == false)
		return false;

	SetLurePosition(PlayerReference->GetActorLocation());
	
	return true;
}

bool AAgentController::TrySetLurePosition(FVector _Position)
{
	if (Blackboard->IsVectorValueSet("CurrentLurePosition") == true)
		return false;

	SetLurePosition(_Position);
	
	return true;
}

void AAgentController::SetLurePosition(FVector _Position)
{
	Blackboard->SetValueAsVector("CurrentLurePosition", _Position);
	InLureState = true;
}

void AAgentController::ClearLurePosition()
{
	Blackboard->ClearValue("CurrentLurePosition");
	InLureState = false;
}

bool AAgentController::TryLoseTrackOfChasedPlayer(float _DeltaTime)
{
	if (SuspicionLevel != 2)
		return false;

	if (WillLosePlayerTrack == false)
		return false;

	CurrentTrackDecreaseTime -= _DeltaTime;
	if (CurrentTrackDecreaseTime <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("LOST TRACE OF PLAYER")));
		PlayerTracked = false;
		WillLosePlayerTrack = false;
	}

	return true;
}

bool AAgentController::TryUpdateChasedPlayerPosition()
{
	if (SuspicionLevel != 2)
		return false;

	if (PlayerTracked == false)
		return false;

	Blackboard->SetValueAsVector("PlayerKnownPosition", PlayerReference->GetActorLocation());

	return true;
}

bool AAgentController::TrySetChasedPlayer()
{
	if (SuspicionLevel != 2)
		return false;

	Blackboard->SetValueAsBool("PlayerDetected", true);

	PlayerTracked = true;

	return true;
}

bool AAgentController::TryClearChasedPlayer()
{
	if (SuspicionLevel == 2)
		return false;

	ClearChasedPlayer();

	PlayerTracked = false;
	
	return true;
}

void AAgentController::ClearChasedPlayer()
{
	Blackboard->ClearValue("PlayerDetected");
}

void AAgentController::IncreaseTimeline()
{
	if (SuspicionLevel >= 2)
		return;

	OnTimelineRestart(0);
}

void AAgentController::DecreaseTimeline()
{
	if (SuspicionLevel <= 0)
		return;

	OnTimelineRestart(1);
}

void AAgentController::OnDetectionTimelineFinished()
{
	if (PlayerVisible == true)
	{
		IncreaseSuspicion();
		TrySetChasedPlayer();
		
		IncreaseTimeline();
	}
	else
	{
		DecreaseSuspicion();
		SetDetectionVisibility(false);
		TryClearChasedPlayer();
		
		DecreaseTimeline();
	}
}

void AAgentController::UpdatePerception(AActor* _Actor, FAIStimulus _Stimulus)
{
	if (ControlledAgent->GetIsDead() == true)
		return;

	if (PlayerReference == _Actor)
	{
		PlayerSensed = _Stimulus.WasSuccessfullySensed();
	}
	
	AAgentCharacter* OtherAgent = dynamic_cast<AAgentCharacter*>(_Actor);
	if (OtherAgent != nullptr)
	{
		TryDetectDeadBody(OtherAgent);
	}
}

void AAgentController::UpdatePlayerVisible()
{
	bool PlayerVisibilityStatus = PlayerSensed == true && PlayerReference->GetIsDead() == false && PlayerReference->IsHidden() == false;
	if (PlayerVisible != PlayerVisibilityStatus)
	{
		PlayerVisible = PlayerVisibilityStatus;
		
		if (PlayerVisible == true)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("PLAYER VISIBLE")));
			SetDetectionVisibility(true);
			WillLosePlayerTrack = false;
		}
		else if (SuspicionLevel == 2 && WillLosePlayerTrack == false)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("PLAYER NOT VISIBLE ANYMORE")));
			WillLosePlayerTrack = true;
			CurrentTrackDecreaseTime = TrackDecreaseRate;
		}

		ControlledAgent->CanSeePlayer = PlayerVisible;
	}
}

bool AAgentController::TryDetectDeadBody(AAgentCharacter* _AgentCharacter)
{
	if (_AgentCharacter->GetIsDead() == false || _AgentCharacter->IsHidden() == true || DeadAgentsCache.Contains(_AgentCharacter->GetName()) == true)
		return false;

	FVector LurePosition = MathUtility::GetPositionAtDistance(_AgentCharacter->GetActorLocation(), ControlledAgent->GetActorLocation(), StopOffsetFromLurePosition);
	TrySetLurePosition(LurePosition);

	DeadAgentsCache.Emplace(_AgentCharacter->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("A DEAD BODY !")));

	return true;
}

void AAgentController::OnPlayerDeath()
{
	ChangeSuspicion(0);

	ClearChasedPlayer();
	ClearLurePosition();
}

void AAgentController::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (ControlledAgent->GetIsDead() == true)
		return;

	UpdatePlayerVisible();

	TryUpdateChasedPlayerPosition();
	TryLoseTrackOfChasedPlayer(_DeltaTime);
	
	TrySetPlayerAsLurePosition();
	
	UpdateDetectionMeterAngle();
	UpdateDetectionTimeline();
}

void AAgentController::InitBehavior()
{
	Run();
	
	Blackboard->SetValueAsFloat("WaitTimeBeforeShoot", WaitBeforeShoot);
	Blackboard->SetValueAsFloat("WaitTimeAfterShoot", WaitAfterShoot);

	Blackboard->SetValueAsObject("PlayerReference", PlayerReference);
}
