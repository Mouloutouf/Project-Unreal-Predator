// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentController.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "ProjectDishonored/Gameplay/Items/Gun.h"
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
	OnIncreaseSuspicion(SuspicionLevel);
	
	ChangeSuspicion(SuspicionLevel + 1);
}

void AAgentController::OnIncreaseSuspicion(int _PreviousSuspicion)
{
}

void AAgentController::DecreaseSuspicion()
{
	OnDecreaseSuspicion(SuspicionLevel);
	
	ChangeSuspicion(SuspicionLevel - 1);
}

void AAgentController::OnDecreaseSuspicion(int _PreviousSuspicion)
{
	if (_PreviousSuspicion == 2)
	{
		SetDistractionLure(EDistractionType::LosingSightPlayer, PlayerLastKnownPosition);
	}
}

void AAgentController::ChangeSuspicion(int _NewSuspicion)
{
	if (_NewSuspicion < 0 || _NewSuspicion > 2)
		return;

	SuspicionLevel = _NewSuspicion;
	
	UpdateDisplayedSuspicionUI();
}

void AAgentController::SetDistractionLure(EDistractionType _DistractionType, FVector _Position)
{
	// TODO Create a Service to Update this Position during Lure
	CurrentLureDestination = _Position;
	
	if (CurrentDistraction == _DistractionType)
		return;
	
	if (_DistractionType != EDistractionType::None)
	{
		SetFirstLureInterruption(_DistractionType);
		
		CurrentLureSpeed = DistractionLureSpeeds[_DistractionType];
	}
	
	CurrentDistraction = _DistractionType;
}

void AAgentController::SetFirstLureInterruption(EDistractionType _DistractionType)
{
	EInterruptionType InterruptionType = EInterruptionType::Blank;
		
	switch (_DistractionType)
	{
	case SeeingPlayer:
	case SeeingDeadBody:
		InterruptionType = EInterruptionType::Light;
		break;
	case HearingGunshot:
		InterruptionType = EInterruptionType::Strong;
		break;
	case LosingSightPlayer:
		InterruptionType = EInterruptionType::PlayerLost;
		break;
	case None:
		break;
	}
		
	SetInterruption(InterruptionType, InterruptionWaitTimes[InterruptionType]);
}

void AAgentController::SetFinalLureInterruption(EDistractionType _DistractionType)
{
	EInterruptionType InterruptionType = EInterruptionType::Blank;
		
	switch (_DistractionType)
	{
	case SeeingPlayer:
	case HearingGunshot:
	InterruptionType = EInterruptionType::NothingFound;
		break;
	case SeeingDeadBody:
		InterruptionType = EInterruptionType::BodyFound;
		break;
	case LosingSightPlayer:
		InterruptionType = EInterruptionType::Blank;
		break;
	case None:
		break;
	}
		
	SetInterruption(InterruptionType, InterruptionWaitTimes[InterruptionType]);
}

void AAgentController::SetInterruption(EInterruptionType _InterruptionType, float _WaitTime)
{
	IsInterrupted = true;
	
	CurrentDistractionVoicelineType = _InterruptionType == EInterruptionType::Blank ? FString() : InterruptionVoicelineTypes[_InterruptionType];
	CurrentInterruptionWaitTime = _WaitTime > 0 ? _WaitTime : DefaultInterruptionWaitTime;
}

void AAgentController::SetInvestigation()
{
	// TODO Investigation Behaviour
}

void AAgentController::TimerBeforeLosingTrackOfPlayer(float _DeltaTime)
{
	if (SuspicionLevel != 2)
		return;

	if (WillLosePlayerTrack == false)
		return;

	CurrentTrackDecreaseTime -= _DeltaTime;
	if (CurrentTrackDecreaseTime <= 0)
	{
		PlayerTracked = false;
		WillLosePlayerTrack = false;
	}
}

void AAgentController::TimerBeforeJoiningChaseOfPlayer(float _DeltaTime)
{
	if (WillJoinPlayerChase == false)
		return;

	CurrentJoinChaseIncreaseTime += _DeltaTime;
	if (CurrentJoinChaseIncreaseTime >= JoinChaseIncreaseRate)
	{
		ChangeSuspicion(2);
		PlayerTracked = true;
		OnTimelineRestart(0.5);
		
		WillJoinPlayerChase = false;
	}
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
		IncreaseTimeline();
	}
	else
	{
		DecreaseSuspicion();
		SetDetectionVisibility(false);
		DecreaseTimeline();
	}
}

void AAgentController::UpdatePerception(AActor* _Actor, FAIStimulus _Stimulus)
{
	if (ControlledAgent->GetIsDead() == true)
		return;

	// Try Sense Player
	if (PlayerReference == _Actor)
	{
		PlayerSensed = _Stimulus.WasSuccessfullySensed();
	}
	
	// Try Sense Agent
	AAgentCharacter* OtherAgent = dynamic_cast<AAgentCharacter*>(_Actor);
	if (OtherAgent != nullptr)
	{
		// Check if In Chase
		if (OtherAgent->IsAttackingPlayer == true)
		{
			if (SuspicionLevel != 2 && WillJoinPlayerChase == false && _Stimulus.WasSuccessfullySensed() == true)
			{
				WillJoinPlayerChase = true;
				CurrentJoinChaseIncreaseTime = 0;

				ChangeSuspicion(1);
			}
			else if (_Stimulus.WasSuccessfullySensed() == false)
			{
				WillJoinPlayerChase = false;
				
				ChangeSuspicion(0);
			}
		}
		
		// Check if Dead
		if (OtherAgent->GetIsDead() == true && OtherAgent->IsHidden() == false && DeadAgentsCache.Contains(OtherAgent->GetName()) == false)
		{
			FVector LurePosition = MathUtility::GetPositionAtDistance(OtherAgent->GetActorLocation(), ControlledAgent->GetActorLocation(), DistractionLureOffsetFromPosition);
			SetDistractionLure(EDistractionType::SeeingDeadBody, LurePosition);

			DeadAgentsCache.Emplace(OtherAgent->GetName());
		}
	}

	// Try Sense Gunshot
	AGun* Gun = dynamic_cast<AGun*>(_Actor);
	if (Gun != nullptr)
	{
		if (_Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
		{
			SetDistractionLure(EDistractionType::HearingGunshot, _Stimulus.StimulusLocation);
		}
	}
}

void AAgentController::UpdatePlayerVisible()
{
	bool PlayerVisibilityStatus = PlayerSensed == true && PlayerReference->GetIsDead() == false && PlayerReference->IsHidden() == false;
	if (PlayerVisible != PlayerVisibilityStatus)
	{
		PlayerVisible = ControlledAgent->CanSeePlayer = PlayerVisibilityStatus;

		if (PlayerVisible == true)
		{
			SetDetectionVisibility(true);
		}
	}

	bool ShouldTrackPlayer = SuspicionLevel == 2 && PlayerVisible == true;
	if (PlayerTracked != ShouldTrackPlayer)
	{
		if (ShouldTrackPlayer == true)
		{
			WillLosePlayerTrack = false;
			PlayerTracked = true;
		}
		else if (WillLosePlayerTrack == false)
		{
			WillLosePlayerTrack = true;
			CurrentTrackDecreaseTime = TrackDecreaseRate;
		}
	}

	bool ShouldBeDistracted = SuspicionLevel == 1 && PlayerVisible == true;
	if (ShouldBeDistracted == true)
	{
		SetDistractionLure(EDistractionType::SeeingPlayer, PlayerReference->GetActorLocation());
	}
}

void AAgentController::OnPlayerDeath()
{
	IsInterrupted = false;
	CurrentDistraction = EDistractionType::None;
	
	ChangeSuspicion(0);
	ControlledAgent->IsAttackingPlayer = false;
}

void AAgentController::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);

	if (ControlledAgent->GetIsDead() == true)
		return;

	UpdatePlayerVisible();
	
	UpdateDetectionMeterAngle();
	UpdateDetectionTimeline();

	TimerBeforeLosingTrackOfPlayer(_DeltaTime);
	TimerBeforeJoiningChaseOfPlayer(_DeltaTime);
}

void AAgentController::InitBehavior()
{
	Run();

	// TODO Put these in a Begin Sequence Task with a Decorator to Check if Behaviour Tree is Initialized
	Blackboard->SetValueAsObject("PlayerReference", PlayerReference);

	Blackboard->SetValueAsEnum("CurrentBehaviourStatus", EBehaviourStatus::Patrol);
	
	Blackboard->SetValueAsFloat("WaitTimeBeforeShoot", AttackWaitBeforeShoot);
	Blackboard->SetValueAsFloat("WaitTimeAfterShoot", AttackWaitAfterShoot);
}
