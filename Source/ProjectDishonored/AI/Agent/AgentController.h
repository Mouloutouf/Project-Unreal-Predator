// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgentCharacter.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AgentController.generated.h"

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
	UFUNCTION()
	void OnSuspicionChanged();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateDisplayedSuspicionUI();

	UFUNCTION()
	bool TrySetPlayerAsLurePosition();
	UFUNCTION()
	bool TrySetLurePosition(FVector _Position);
	UFUNCTION()
	void SetLurePosition(FVector _Position);
	UFUNCTION(BlueprintCallable)
	void ClearLurePosition();

	UFUNCTION()
	bool TrySetChasedPlayer() const;
	UFUNCTION()
	bool TryClearChasedPlayer() const;
	UFUNCTION()
	void ClearChasedPlayer() const;

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
	void UpdatePlayerDetected();
	UFUNCTION()
	bool TryDetectDeadBody(AAgentCharacter* _AgentCharacter);

	UFUNCTION()
	void OnPlayerDeath();
	
	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* ControlledAgent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerCharacter* PlayerReference;

	// TODO Make the Suspicion Level an Enum
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int SuspicionLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PlayerSensed = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PlayerDetected = false;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerNormalDetectionRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerSprintingDetectionRate = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerInProneDetectionRate = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool InLureState = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StopOffsetFromLurePosition = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<FString> DeadAgentsCache;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WaitBeforeShoot = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WaitAfterShoot = 2;
	
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

	UFUNCTION()
	void SetDeathStatus() const;
	
	bool GetPlayerDetected() const { return PlayerDetected; }
	int GetSuspicionLevel() const { return SuspicionLevel; }
};
