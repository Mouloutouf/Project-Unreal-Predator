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

TKeyValuePair<int, USoundBase*> UAgentVoice::GetRandomVoiceline(const TArray<TKeyValuePair<int, USoundBase*>>& _Voicelines)
{
	int r = UKismetMathLibrary::RandomInteger(_Voicelines.Num());
	return _Voicelines[r];
}

TArray<TKeyValuePair<int, USoundBase*>> UAgentVoice::GetAvailableVoicelines(FString _VoicelineKey)
{
	const FSoundArray& Voicelines = AgentSoundData->Voicelines[_VoicelineKey];
	
	TArray<TKeyValuePair<int, USoundBase*>>&& AvailableVoicelines = TArray<TKeyValuePair<int, USoundBase*>>();
	for (int i = 0; i < Voicelines.Sounds.Num(); ++i)
	{
		if (i == UnavailableVoicelineAtIndex.Value)
			continue;

		AvailableVoicelines.Add({ i, Voicelines.Sounds[i] });
	}

	return AvailableVoicelines;
}

void UAgentVoice::TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction)
{
	Super::TickComponent(_DeltaTime, _TickType, _ThisTickFunction);

	if (SoundPlaying == true)
	{
		CurrentSoundTime -= _DeltaTime;
		if (CurrentSoundTime <= 0)
		{
			SoundPlaying = false;
		}
	}
}

// TODO Return a Boolean to Know if the Voiceline was indeed Played
void UAgentVoice::PlayVoiceline(FString _VoicelineKey, float _Delay)
{
	if (AgentSoundData == nullptr)
		return;
	
	if (AgentSoundData->Voicelines.Contains(_VoicelineKey) == true)
	{
		TArray<TKeyValuePair<int, USoundBase*>> AvailableVoicelines = GetAvailableVoicelines(_VoicelineKey);
		TKeyValuePair<int, USoundBase*> RandomVoiceline = GetRandomVoiceline(AvailableVoicelines);
		UnavailableVoicelineAtIndex = { _VoicelineKey, RandomVoiceline.Key };

		if (_Delay > 0)
		{
			PlaySoundWithDelay(RandomVoiceline.Value, _Delay);
		}
		else
		{
			PlaySound(RandomVoiceline.Value);
		}
	}
}

void UAgentVoice::PlaySound(USoundBase* _Sound)
{
	if (CanPlaySound == false)
		return;

	StopCurrentVoiceline();
	
	CurrentPlayingAudio = UGameplayStatics::SpawnSoundAtLocation(this, _Sound, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, 1, 1, 0, SoundAttenuation);
	
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
