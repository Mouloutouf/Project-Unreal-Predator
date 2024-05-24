#pragma once

#include "CoreMinimal.h"
#include "AgentSoundData.generated.h"

USTRUCT(BlueprintType)
struct PROJECTDISHONORED_API FSoundArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<USoundBase*> Sounds;
};

USTRUCT(BlueprintType)
struct PROJECTDISHONORED_API FAgentSoundData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FString, FSoundArray> Voicelines;
};
