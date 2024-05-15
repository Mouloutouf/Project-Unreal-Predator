// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectDishonored/Utility/MathUtility.h"

void AAgentController::BeginPlay()
{
	Super::BeginPlay();

	ControlledAgent = dynamic_cast<AAgentCharacter*>(GetPawn());

	CurrentDetectionRate =  BaseDetectionRate;
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
	float MaxIncreaseRate = SuspicionLevel == 0 ? MaxLureIncreaseRate : MaxSuspicionIncreaseRate;
	float MinIncreaseRate = SuspicionLevel == 0 ? MinLureIncreaseRate : MinSuspicionIncreaseRate;
	
	float Distance = FVector::Distance(ControlledAgent->GetActorLocation(), PlayerReference->GetActorLocation());
	// TODO Arbitrary value what the fuck is this
	float Alpha = Distance / 1500;
	
	float RateByDistance = UKismetMathLibrary::Lerp(MaxIncreaseRate, MinIncreaseRate, Alpha);
	return 1 / (RateByDistance * CurrentDetectionRate);
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
	ControlledAgent->CurrentSuspicionLevel = SuspicionLevel;

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

	if (PlayerDetected == false)
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

bool AAgentController::TrySetChasedPlayer() const
{
	if (SuspicionLevel != 2)
		return false;

	Blackboard->SetValueAsObject("DetectedPlayer", PlayerReference);

	return true;
}

bool AAgentController::TryClearChasedPlayer() const
{
	if (SuspicionLevel == 2)
		return false;

	ClearChasedPlayer();

	return true;
}

void AAgentController::ClearChasedPlayer() const
{
	Blackboard->ClearValue("DetectedPlayer");
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
	if (PlayerDetected == true)
	{
		IncreaseSuspicion();
		
		if (TrySetChasedPlayer() == true)
		{
			IncreaseTimeline();
		}
	}
	else
	{
		DecreaseSuspicion();
		SetDetectionVisibility(false);
		
		if (TryClearChasedPlayer() == true)
		{
			DecreaseTimeline();
		}
	}
}

bool AAgentController::TryUpdateDetectionRate()
{
	int PlayerSprintState = PlayerReference->GetIsSprinting() ? 1 : 0;
	int PlayerCrouchState = PlayerReference->GetIsInProne() ? -1 : 0;

	int NewPlayerState = PlayerSprintState + PlayerCrouchState;
	
	if (NewPlayerState == CurrentPlayerState)
		return false;

	CurrentPlayerState = NewPlayerState;
	SetDetectionRate(CurrentPlayerState);

	return true;
}

void AAgentController::SetDetectionRate(int _PlayerState)
{
	CurrentDetectionRate = _PlayerState == -1 ? CrouchDetectionRate : SuspicionLevel == 1 ? RunDetectionRate : BaseDetectionRate;
}

void AAgentController::UpdatePerception(AActor* _Actor, FAIStimulus _Stimulus)
{
	if (ControlledAgent->GetIsDead() == true)
		return;

	if ((AActor*)PlayerReference == _Actor)
	{
		PlayerSensed = _Stimulus.WasSuccessfullySensed();
	}
	
	AAgentCharacter* OtherAgent = dynamic_cast<AAgentCharacter*>(_Actor);
	if (OtherAgent != nullptr)
	{
		TryDetectDeadBody(OtherAgent);
	}
}

void AAgentController::UpdatePlayerDetected()
{
	bool PlayerDetectedStatus = PlayerSensed == true && PlayerReference->GetIsDead() == false && PlayerReference->IsHidden() == false;
	if (PlayerDetected != PlayerDetectedStatus)
	{
		PlayerDetected = PlayerDetectedStatus;
		ControlledAgent->IsPlayerDetected = PlayerDetected;
		
		if (PlayerDetected == true)
		{
			SetDetectionVisibility(true);
		}
	}
}

bool AAgentController::TryDetectDeadBody(AAgentCharacter* _AgentCharacter)
{
	if (_AgentCharacter->GetIsDead() == false || _AgentCharacter->IsHidden() == true || DeadAgentsCache.Contains(_AgentCharacter->GetName()) == true)
		return false;

	// TODO Arbitrary distance value cannot have that
	FVector LurePosition = MathUtility::GetPositionAtDistance(_AgentCharacter->GetActorLocation(), ControlledAgent->GetActorLocation(), 100);
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

	UpdatePlayerDetected();
	
	TrySetPlayerAsLurePosition();
	TryUpdateDetectionRate();
	
	UpdateDetectionMeterAngle();
	UpdateDetectionTimeline();
}

void AAgentController::Init()
{
	PlayerReference->OnPlayerDeath.AddDynamic(this, &AAgentController::OnPlayerDeath);
	
	Blackboard->SetValueAsFloat("WaitTimeBeforeShoot", WaitBeforeShoot);
	Blackboard->SetValueAsFloat("WaitTimeAfterShoot", WaitAfterShoot);
	
	ControlledAgent->CurrentSuspicionLevel = SuspicionLevel = 0;
	OnSuspicionChanged();

	SetTimelinePlayRate(1 / MinSuspicionIncreaseRate);
}
