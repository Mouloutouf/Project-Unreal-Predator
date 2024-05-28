// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AgentSoundData.h"
#include "Components/ActorComponent.h"
#include "AgentVoice.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTDISHONORED_API UAgentVoice : public UActorComponent
{
	GENERATED_BODY()

public:
	UAgentVoice();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	static USoundBase* GetRandomVoiceline(FSoundArray& _VoicelineArray);
	UFUNCTION()
	void PlaySound(USoundBase* _Sound);
	UFUNCTION()
	void PlaySoundWithDelay(USoundBase* _Sound, float _Delay);
	
public:
	virtual void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void PlayVoiceline(FString _VoicelineKey, float _Delay = 0);
	UFUNCTION()
	void StopCurrentVoiceline() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanPlaySound = true;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAgentSoundData* AgentSoundData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent* CurrentPlayingAudio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SoundPlaying;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentSoundTime;
};
