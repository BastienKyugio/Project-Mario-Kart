#include "Settings/KartGameUserSettings.h"

#include "Kismet/GameplayStatics.h"
#include "Settings/KartAudioSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

UKartGameUserSettings* UKartGameUserSettings::Get()
{
	return Cast<UKartGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

void UKartGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();
	
	MasterVolume = 1.f;
	MusicVolume = 0.8f;
	SfxVolume = 1.f;
	CameraFieldOfView = 90.f;
	LapCount = 3;
}

void UKartGameUserSettings::SetMasterVolume(float Value)
{
	MasterVolume = FMath::Clamp(Value, 0.f, 1.f);
}

void UKartGameUserSettings::SetMusicVolume(float Value)
{
	SfxVolume = FMath::Clamp(Value, 0.f, 1.f);
}

void UKartGameUserSettings::ApplyAudioSettings(const UObject* WorldContext) const
{
	const UKartAudioSettings* AudioSettings = GetDefault<UKartAudioSettings>();
	
	USoundMix* SoundMix = AudioSettings->SoundMix.LoadSynchronous();
	if (SoundMix == nullptr)
	{
		return;
	}
	
	ApplyClassVolume(WorldContext, SoundMix, AudioSettings->MusicSoundClass.LoadSynchronous(), MasterVolume * MusicVolume);
	ApplyClassVolume(WorldContext, SoundMix, AudioSettings->SfxSoundClass.LoadSynchronous(), MasterVolume * SfxVolume);
	
	UGameplayStatics::PushSoundMixModifier(WorldContext, SoundMix);
}


void UKartGameUserSettings::ApplyClassVolume(const UObject* WorldContext, USoundMix* SoundMix, USoundClass* SoundClass, float Volume)
{
	if (SoundClass == nullptr)
	{
		return;
	}
	
	UGameplayStatics::SetSoundMixClassOverride(WorldContext, SoundMix, SoundClass, Volume, 1.f, 0.f, true);
}

void UKartGameUserSettings::SetCameraFieldOfView(float Value)
{
	CameraFieldOfView = FMath::Clamp(Value, MinFieldOfView, MaxFieldOfView);
}

void UKartGameUserSettings::SetLapCount(int Value)
{
	LapCount = FMath::Clamp(Value, MinLapCount, MaxLapCount);
}
