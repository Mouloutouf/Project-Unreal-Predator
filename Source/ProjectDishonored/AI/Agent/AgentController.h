// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgentCharacter.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AgentController.generated.h"

UENUM(BlueprintType)
enum EBehaviourStatus { Patrol, Lure, Investigation, Attack };

UENUM(BlueprintType)
enum EDistractionType { None, SeeingPlayer, SeeingDeadBody, HearingGunshot, LosingSightPlayer };

UENUM(BlueprintType)
enum EInterruptionType { Blank, Light, Strong, NothingFound, BodyFound, PlayerLost };

UCLASS()
class PROJECTDISHONORED_API AAgentController : public AAIController
{
	GENERATED_BODY()

public:
	AAgentController() = default;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void UpdateDetectionMeterAngle() const;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetDetectionIncreaseRate();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetDetectionDecreaseRate();

	UFUNCTION()
	void IncreaseSuspicion();
	UFUNCTION()
	void DecreaseSuspicion();
	UFUNCTION()
	void ChangeSuspicion(int _Value);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateDisplayedSuspicionUI();

	UFUNCTION(BlueprintCallable)
	void SetDistractionLure(EDistractionType _DistractionType, FVector _Position = FVector::ZeroVector);
	UFUNCTION(BlueprintCallable)
	void SetFirstLureInterruption(EDistractionType _DistractionType);
	UFUNCTION(BlueprintCallable)
	void SetFinalLureInterruption(EDistractionType _DistractionType);
	UFUNCTION(BlueprintCallable)
	void SetInterruption(EInterruptionType _InterruptionType, float _WaitTime = 0);
	UFUNCTION()
	void SetInvestigation();

	UFUNCTION()
	bool TryLoseTrackOfChasedPlayer(float _DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetAimedStatus(bool _Status);
	
	UFUNCTION()
	void IncreaseTimeline();
	UFUNCTION()
	void DecreaseTimeline();

	UFUNCTION(BlueprintImplementableEvent)
	void SetTimelinePlayRate(float _NewRate);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTimelineRestart(float _NewTime);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateDetectionTimeline();
	UFUNCTION(BlueprintCallable)
	void OnDetectionTimelineFinished();

	UFUNCTION(BlueprintCallable)
	void UpdatePerception(AActor* _Actor, FAIStimulus _Stimulus);
	UFUNCTION()
	void UpdatePlayerVisible();

	UFUNCTION()
	void OnPlayerDeath();
	
	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* ControlledAgent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerCharacter* PlayerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int SuspicionLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EBehaviourStatus> BehaviourStatus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PlayerSensed = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PlayerVisible = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PlayerTracked = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxLureIncreaseRate = 0.0001;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinLureIncreaseRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxSuspicionIncreaseRate = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinSuspicionIncreaseRate = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float IncreaseRateDistanceRatio = 1500; // TODO This should be equal to the detection distance of the Agent
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LureDecreaseRate = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SuspicionDecreaseRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChaseDecreaseRate = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TrackDecreaseRate = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool WillLosePlayerTrack = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentTrackDecreaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerNormalDetectionRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerSprintingDetectionRate = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerInProneDetectionRate = 2;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* DetectionMeterWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsDetectionMeterVisible = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrolSpeed = 200;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LureSpeed = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChaseSpeed = 400;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<EDistractionType> CurrentDistraction;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsInterrupted = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<EDistractionType>, float> DistractionLureSpeeds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<EDistractionType>, float> DistractionLureWaitTimes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<EInterruptionType>, FString> InterruptionVoicelineTypes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultInterruptionWaitTime = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CurrentDistractionVoicelineType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentLureDestination;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentLureWaitTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentLureSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentInterruptionWaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DistractionLureOffsetFromPosition = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<FString> DeadAgentsCache;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackWaitBeforeShoot = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackWaitAfterShoot = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString AttackPlayerVisibleVoicelineType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString AttackPlayerNotVisibleVoicelineType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString AttackPlayerNotTrackedVoicelineType;
	
public:
	virtual void Tick(float _DeltaTime) override;
	
	void InitBehavior();

	UFUNCTION(BlueprintImplementableEvent)
	void Run();
	UFUNCTION()
	void Stop() const;

	UFUNCTION(BlueprintImplementableEvent)
	void SetDetectionVisibility(bool _Visibility);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDetectionProgress(float _Progress);
	
	bool GetPlayerVisible() const { return PlayerVisible; }
	bool GetPlayerTracked() const { return PlayerTracked; }
	
	int GetSuspicionLevel() const { return SuspicionLevel; }
};
