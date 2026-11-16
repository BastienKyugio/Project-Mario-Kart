#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "KartAudioSettings.generated.h"

class USoundMix;
class USoundClass;

UCLASS(Config= Game, DefaultConfig, meta = (DisplayName = "KartAudio"))
class PROJECTMARIOKART_API UKartAudioSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Audio")
	TSoftObjectPtr<USoundMix> SoundMix;
	
	UPROPERTY(Config, EditAnywhere, Category = "Audio")
	TSoftObjectPtr<USoundClass> MusicSoundClass;
	
	UPROPERTY(Config, EditAnywhere, Category = "Audio")
	TSoftObjectPtr<USoundClass> SfxSoundClass;
	
};
