// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentVoice.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UAgentVoice::UAgentVoice()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAgentVoice::BeginPlay()
{
	Super::BeginPlay();
}

USoundBase* UAgentVoice::GetRandomVoiceline(FSoundArray& _VoicelineArray)
{
	int r = UKismetMathLibrary::RandomInteger(_VoicelineArray.Sounds.Num());
	return _VoicelineArray.Sounds[r];
}

void UAgentVoice::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

	if (SoundPlaying == true)
	{
		CurrentSoundTime -= _DeltaTime;
		if (CurrentSoundTime <= 0)
			SoundPlaying = false;
	}
}

void UAgentVoice::PlayVoiceline(FString _VoicelineKey, float _Delay)
{
	if (AgentSoundData == nullptr)
		return;
	
	if (AgentSoundData->Voicelines.Contains(_VoicelineKey) == true)
	{
		StopCurrentVoiceline();
		
		USoundBase* RandomVoiceline = GetRandomVoiceline(AgentSoundData->Voicelines[_VoicelineKey]);

		if (_Delay > 0)
		{
			PlaySoundWithDelay(RandomVoiceline, _Delay);
		}
		else
		{
			PlaySound(RandomVoiceline);
		}
	}
}

void UAgentVoice::PlaySound(USoundBase* _Sound)
{
	CurrentPlayingAudio = UGameplayStatics::SpawnSoundAtLocation(this, _Sound, GetOwner()->GetActorLocation());
	
	SoundPlaying = true;
	CurrentSoundTime = _Sound->Duration;
}

void UAgentVoice::PlaySoundWithDelay(USoundBase* _Sound, float _Delay)
{
	FTimerDelegate PlaySoundDelegate = FTimerDelegate::CreateUObject(this, &UAgentVoice::PlaySound, _Sound);
	FTimerHandle TimerHandle;
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle, PlaySoundDelegate, _Delay, false);
}

void UAgentVoice::StopCurrentVoiceline() const
{
	if (SoundPlaying == true)
	{
		CurrentPlayingAudio->SetActive(false);
	}
}
