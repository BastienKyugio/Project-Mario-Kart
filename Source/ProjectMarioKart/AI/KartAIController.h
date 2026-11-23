#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KartAIController.generated.h"

class AKartPawn;
class AKartRaceTrack;
class UKartDriverProfile;

UCLASS()
class PROJECTMARIOKART_API AKartAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AKartAIController();
	
	virtual void Tick(float DeltaTime) override;
	
	void SetDriverProfile(UKartDriverProfile* InProfile);
	
	UKartDriverProfile* GetDriverProfile() const { return Profile; }
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Volant", meta=(ClampMin="100"))
	float LookAheadMin = 700.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Volant", meta=(ClampMin="0"))
	float LookAheadPerSpeed = 0.4f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Volant", meta=(ClampMin="1"))
	float FullSteerAngle = 30.f;
	
private:
	void UpdateProgress();
	
	void UpdateSteering(float DeltaTime);
	
	void UpdateThrottleAndDrift();
	
	const UKartDriverProfile* GetProfileOrDefault() const;
	
	UPROPERTY()
	TObjectPtr<UKartDriverProfile> Profile;
	
	UPROPERTY()
	TObjectPtr<AKartRaceTrack> Track;
	
	UPROPERTY()
	TObjectPtr<AKartPawn> Kart;
	
	float TrackDistance = 0.f;
	
	float WanderTime = 0.f;
	
	float CurrentSteer = 0.f;
};