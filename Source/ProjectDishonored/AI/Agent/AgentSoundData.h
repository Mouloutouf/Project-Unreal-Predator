﻿#pragma once

#include "CoreMinimal.h"
#include "AgentSoundData.generated.h"

USTRUCT(BlueprintType)
struct PROJECTDISHONORED_API FSoundArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USoundBase*> Sounds;
};

UCLASS()
class PROJECTDISHONORED_API UAgentSoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, FSoundArray> Voicelines;
};
