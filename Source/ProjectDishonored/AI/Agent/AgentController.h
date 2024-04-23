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
	// Sets default values for this character's properties
	AAgentController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// TODO Put these methods in a Utility script
	UFUNCTION(BlueprintCallable)
	float AngleBetweenVectors(FVector _FirstVector, FVector _SecondVector);
	UFUNCTION(BlueprintPure, BlueprintCallable)
	FVector GetPositionAtDistance(FVector _ClosestPosition, FVector _FurthestPosition, float _Distance);

	UFUNCTION(BlueprintCallable)
	void UpdateDetectionMeterAngle();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetDetectionIncreaseRate();
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetDetectionDecreaseRate();

	UFUNCTION(BlueprintCallable)
	bool TryIncreaseSuspicion();
	UFUNCTION(BlueprintCallable)
	bool TryDecreaseSuspicion();
	UFUNCTION()
	bool TryChangeSuspicion(int _Value);
	UFUNCTION(BlueprintCallable)
	void UpdateSuspicion();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateDisplayedSuspicionUI();

	UFUNCTION(BlueprintCallable)
	bool TryUpdateLurePosition();
	UFUNCTION(BlueprintCallable)
	bool TrySetLurePosition(FVector _Position);
	UFUNCTION(BlueprintCallable)
	void SetLurePosition(FVector _Position);

	UFUNCTION(BlueprintCallable)
	bool TrySetChasedPlayer();
	UFUNCTION(BlueprintCallable)
	bool TryClearChasedPlayer();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetAimedStatus(bool _Status);
	
	UFUNCTION(BlueprintCallable)
	void IncreaseTimeline();
	UFUNCTION(BlueprintCallable)
	void DecreaseTimeline();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnTimelineRestart(float _NewTime);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTimelinePlayRate(float _NewRate);
	
	UFUNCTION(BlueprintCallable)
	bool TryUpdateDetectionRate();
	UFUNCTION(BlueprintCallable)
	void SetDetectionRate(int _PlayerState);

	UFUNCTION(BlueprintCallable)
	bool TryDetectDeadBody(AAgentCharacter* _AgentCharacter);

	UFUNCTION(BlueprintCallable)
	void UpdatePerception(AActor* _Actor, FAIStimulus _Stimulus);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerDetected();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateDetectionTimeline();

	UFUNCTION(BlueprintCallable)
	void OnDetectionTimelineFinished();
	
	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AAgentCharacter* ControlledAgent;

	// TODO Make the Suspicion Level an Enum
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int SuspicionLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool PlayerSensed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PlayerDetected;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxLureIncreaseRate = 0.0001;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinLureIncreaseRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxSuspicionIncreaseRate = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinSuspicionIncreaseRate = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LureDecreaseRate = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SuspicionDecreaseRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChaseDecreaseRate = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* DetectionMeterWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsDetectionMeterVisible;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrolSpeed = 200;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LureSpeed = 300;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ChaseSpeed = 400;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentDetectionRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDetectionRate = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunDetectionRate = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CrouchDetectionRate = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrentPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool InLureState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSet<FString> DeadAgentsCache;

public:
	// Called every frame
	virtual void Tick(float _DeltaTime) override;
	
	void Initialize();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RunBehavior();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDetectionVisibility(bool _Visibility);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDetectionProgress(float _Progress);

	// TODO Create an Agent Data class to store data that should be known by both the Agent Controller and Agent Character, and to which both of them can write
	bool GetPlayerDetected() const { return PlayerDetected; }
	int GetSuspicionLevel() const { return SuspicionLevel; }

	APlayerCharacter* PlayerReference;
};
