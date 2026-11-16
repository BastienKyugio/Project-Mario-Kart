#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "KartGameUserSettings.generated.h"

class USoundMix;
class USoundClass;

UCLASS()
class PROJECTMARIOKART_API UKartGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	static UKartGameUserSettings* Get();   
	
	virtual void SetToDefaults() override;
	
	float GetMasterVolume() const  { return MasterVolume; }
	void SetMasterVolume(float Value);
	
	float GetMusicVolume() const  { return MusicVolume; }
	void SetMusicVolume(float Value);
	
	float GetSfxVolume() const  { return SfxVolume; }
	void SetSfxVolume(float Value);
	
	void ApplyAudioSettings(const UObject* WorldContext) const;
	
	float GetCameraFieldOfView() const  { return CameraFieldOfView; }
	void SetCameraFieldOfView(float Value);
	
	int32 GetLapCount() const  { return LapCount; }
	void SetLapCount(int32 Value);
	
	static constexpr float MinFieldOfView = 70.f;
	static constexpr float MaxFieldOfView = 110.f;
	static constexpr int32 MinLapCount = 1;
	static constexpr int32 MaxLapCount = 9;
	
protected:
	UPROPERTY(Config)
	float MasterVolume = 1;
	
	UPROPERTY(Config)
	float MusicVolume = 0.8f;
	
	UPROPERTY(Config)
	float SfxVolume = 1.f;
	
	UPROPERTY(Config)
	float CameraFieldOfView = 90.f;
	
	UPROPERTY(Config)
	int32 LapCount = 3;
	
private:
	static void ApplyClassVolume(const UObject* WorldContext, USoundMix* SoundMix, USoundClass* SoundClass, float Volume);
	
};
