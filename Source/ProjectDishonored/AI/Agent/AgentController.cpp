// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AAgentController::AAgentController()
{
}

void AAgentController::BeginPlay()
{
	Super::BeginPlay();

	ControlledAgent = dynamic_cast<AAgentCharacter*>(GetPawn());

	CurrentDetectionRate =  BaseDetectionRate;
}

float AAgentController::AngleBetweenVectors(FVector _FirstVector, FVector _SecondVector)
{
	_FirstVector = _FirstVector.GetSafeNormal();
	_SecondVector = _SecondVector.GetSafeNormal();

	FVector Cross = FVector::CrossProduct(_FirstVector, _SecondVector);
	float Dot = FVector::DotProduct(_FirstVector, _SecondVector);

	float Angle = UKismetMathLibrary::DegAcos(Dot);

	return Cross.Z > 0 ? Angle : -Angle;
}

FVector AAgentController::GetPositionAtDistance(FVector _ClosestPosition, FVector _FurthestPosition, float _Distance)
{
	FVector Direction = (_FurthestPosition - _ClosestPosition).GetSafeNormal();
	return _ClosestPosition + (Direction * _Distance);
}

void AAgentController::UpdateDetectionMeterAngle()
{
	if (UKismetSystemLibrary::IsValid(DetectionMeterWidget) == false)
		return;
	
	FVector AgentPositionFromPlayer = ControlledAgent->GetActorLocation() - ControlledAgent->GetPlayer()->GetActorLocation();
	FVector PlayerDirection = ControlledAgent->GetPlayer()->GetActorForwardVector();
	
	float Angle = -AngleBetweenVectors(AgentPositionFromPlayer, PlayerDirection);
	DetectionMeterWidget->SetRenderTransformAngle(Angle);
}

float AAgentController::GetDetectionIncreaseRate()
{
	float MaxIncreaseRate = SuspicionLevel == 0 ? MaxLureIncreaseRate : MaxSuspicionIncreaseRate;
	float MinIncreaseRate = SuspicionLevel == 0 ? MinLureIncreaseRate : MinSuspicionIncreaseRate;
	
	float Distance = FVector::Distance(ControlledAgent->GetActorLocation(), ControlledAgent->GetPlayer()->GetActorLocation());
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

bool AAgentController::TryIncreaseSuspicion()
{
	return TryChangeSuspicion(SuspicionLevel + 1);
}

bool AAgentController::TryDecreaseSuspicion()
{
	return TryChangeSuspicion(SuspicionLevel - 1);
}

bool AAgentController::TryChangeSuspicion(int _Value)
{
	if (_Value <= 0 || _Value >= 2)
		return false;

	SuspicionLevel = _Value;
	UpdateSuspicion();
	
	return true;
}

void AAgentController::UpdateSuspicion()
{
	UpdateDisplayedSuspicionUI();

	float Speed = SuspicionLevel == 0 ? PatrolSpeed : SuspicionLevel == 1 ? LureSpeed : ChaseSpeed;
	ControlledAgent->ChangeCharacterSpeed(Speed);

	if (ControlledAgent->GetIsDead() == true)
		return;

	// TODO Improve this, does not work very well
	ControlledAgent->PreventTakedown = SuspicionLevel == 2;

	if (ControlledAgent->GetCanTakedown() == false)
		return;

	ControlledAgent->UpdateTakedown(ControlledAgent->PreventTakedown == false);
}

bool AAgentController::TryUpdateLurePosition()
{
	if (SuspicionLevel != 1)
		return false;

	if (PlayerDetected == false)
		return false;

	SetLurePosition(ControlledAgent->GetPlayer()->GetActorLocation());
	
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

bool AAgentController::TrySetChasedPlayer()
{
	if (SuspicionLevel != 2)
		return false;

	Blackboard->SetValueAsObject("DetectedPlayer", ControlledAgent->GetPlayer());

	return true;
}

bool AAgentController::TryClearChasedPlayer()
{
	if (SuspicionLevel >= 2)
		return false;

	Blackboard->ClearValue("DetectedPlayer");

	return true;
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

bool AAgentController::TryUpdateDetectionRate()
{
	int PlayerSprintState = ControlledAgent->GetPlayer()->GetIsSprinting() ? 1 : 0;
	int PlayerCrouchState = ControlledAgent->GetPlayer()->GetIsCrouching() ? -1 : 0;

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

bool AAgentController::TryDetectDeadBody(AAgentCharacter* _AgentCharacter)
{
	if (_AgentCharacter->GetIsDead() == false || DeadAgentsCache.Contains(_AgentCharacter->GetName()) == true)
		return false;

	// TODO Arbitrary distance value cannot have that
	FVector LurePosition = GetPositionAtDistance(_AgentCharacter->GetActorLocation(), ControlledAgent->GetActorLocation(), 100);
	TrySetLurePosition(LurePosition);

	DeadAgentsCache.Emplace(_AgentCharacter->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("A DEAD BODY !")));

	return true;
}

void AAgentController::UpdatePerception(AActor* _Actor, FAIStimulus _Stimulus)
{
	if (ControlledAgent->GetIsDead() == true)
		return;

	APlayerCharacter* Player = dynamic_cast<APlayerCharacter*>(_Actor);
	if (Player != nullptr)
	{
		PlayerDetected = _Stimulus.WasSuccessfullySensed();
		if (PlayerDetected == true)
			SetDetectionVisibility(true);
	}
	
	AAgentCharacter* OtherAgent = dynamic_cast<AAgentCharacter*>(_Actor);
	if (OtherAgent != nullptr)
		TryDetectDeadBody(OtherAgent);
}

void AAgentController::OnDetectionTimelineFinished()
{
	if (PlayerDetected == true)
	{
		bool Success = TryIncreaseSuspicion();
		
		TrySetChasedPlayer();
		
		if (Success)
			IncreaseTimeline();
	}
	else
	{
		bool Success = TryDecreaseSuspicion();
		
		TryClearChasedPlayer();
		SetDetectionVisibility(false);
		
		if (Success)
			DecreaseTimeline();
	}
}

void AAgentController::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (ControlledAgent->GetIsDead() == true)
		return;

	TryUpdateLurePosition();
	TryUpdateDetectionRate();
	
	UpdateDetectionMeterAngle();
	UpdateDetectionTimeline();
}

void AAgentController::Initialize()
{
	SuspicionLevel = 0;
	UpdateSuspicion();

	SetTimelinePlayRate(1 / MinSuspicionIncreaseRate);
}
