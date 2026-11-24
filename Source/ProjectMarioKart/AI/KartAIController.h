#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KartAIController.generated.h"

class AKartPawn;
class AKartRaceTrack;
class UKartDriverProfile;

USTRUCT(BlueprintType)
struct FKartAIDifficulty
{
	GENERATED_BODY()
	
	float SpeedFactor = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0", ClampMax="1"))
	float RubberBandStrength = 0.5;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="-1", ClampMax="1"))
	float PrecisionBonus = 0.f;
	
};

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
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Virage", meta = (ClampMin="100"))
	float CornerLookAhead = 2000.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Virage", meta = (ClampMin="0"))
	float BrakeTurnAngle = 55.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Virage", meta = (ClampMin="0"))
	float CornerSpeed = 1600.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Virage", meta = (ClampMin="0"))
	float DriftMinTurnAngle = 35.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Virage", meta = (ClampMin="0"))
	float DriftMaxTurnAngle = 100.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Couloir", meta = (ClampMin="0", ClampMax="1"))
	float LaneUsage = 0.74f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Couloir", meta = (ClampMin="1"))
	float LaneChangeSpeed = 350.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Depassement", meta = (ClampMin="0"))
	float OvertakeDistance = 1200.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Depassement", meta = (ClampMin="0"))
	float OvertakeOffset = 450.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Secours", meta = (ClampMin="0"))
	float StuckSpeed = 150.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Secours", meta = (ClampMin="0.1"))
	float StuckTime = 1.5f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Secours", meta = (ClampMin="0.1"))
	float ReverseDuration = 1.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Secours", meta=(ClampMin="0.1"))
	float RespawnTime = 5.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Objets", meta = (ClampMin="0"))
	float ProjectileRange = 3000.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Objets", meta = (ClampMin="0"))
	float ProjectileAimAngle = 15.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Objets", meta = (ClampMin="0"))
	float TrapRange = 1500.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Elastique", meta = (ClampMin="0"))
	float RubberBandDeadZone = 1500.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Elastic", meta = (ClampMin="1"))
	float RubberBandMaxGap = 6000.f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Elastique", meta = (ClampMin="1"))
	float RubberBandMaxCorrection = 0.12f;
	
	UPROPERTY(EditAnywhere, Category="Kart|AI|Difficulte")
	TArray<FKartAIDifficulty> DifficultyLevels;
	
private:
	void UpdateProgress();
	
	void UpdateSteering(float DeltaTime);
	
	void UpdateThrottleAndDrift();
	
	void UpdateLane(float DeltaTime);
	
	bool UpdateRecovery(float DeltaTime);
	
	void UpdateItem(float DeltaTime);
	
	void UpdateRubberBand(float DeltaTime);
	
	const UKartDriverProfile* GetProfileOrDefault() const;
	
	AKartPawn* FindNearestKart(bool bAhead, float MaxDistance, float ConeAngle) const;
	
	void RespawnOnTrack();
	
	float GetTotalProgress(const AKartPawn* OtherKart, float& InOutLastDistance, int32& InOutLaps);
	
	const FKartAIDifficulty GetDifficulty() const;
	
	UPROPERTY()
	TObjectPtr<UKartDriverProfile> Profile;
	
	UPROPERTY()
	TObjectPtr<AKartRaceTrack> Track;
	
	UPROPERTY()
	TObjectPtr<AKartPawn> Kart;
	
	float TrackDistance = 0.f;
	
	float WanderTime = 0.f;
	
	float CurrentSteer = 0.f;
	
	float UpComingTurnAngle = 0.f;
	
	float DriftSide = 0.f;

	float CurrentLane = 0.f;
	
	bool bHoldBack = false;
	
	float StuckTimer = 0.f;
	float ReverseTimer = 0.f;
	float RecoveryTimer = 0.f;
	
	float ItemHolderTimer = 0.f;
	
	float MyProgress = 0.f;
	float MyLastDistance = -1.f;
	int32 MyLaps = 0;
	float PlayerLastDistance = -1.f;
	int32 PlayerLaps = 0;
	
	float CurrentSpeedMultiplier = 1.f;
};