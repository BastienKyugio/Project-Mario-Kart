// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KartGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountdownTick, int32, Number);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRaceStarted);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKartFinished, AKartPawn*, Kart);
UCLASS()
class PROJECTMARIOKART_API AKartGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AKartGameMode();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable, Category= "Kart|Countdown")
	FOnCountdownTick OnCountdownTick;
	
	UPROPERTY(BlueprintAssignable, Category= "Kart|Countdown")
	FOnRaceStarted OnRaceStarted;
	
	UPROPERTY(BlueprintAssignable, Category= "Kart|Race")
	FOnKartFinished OnKartFinished;
	
	UFUNCTION(BlueprintPure, Category= "Kart|Countdown")
	bool IsRaceStarted() const { return bRaceStarted || !bUseCountdown; }
	
	UFUNCTION(BlueprintPure, Category= "Kart|Countdown")
	int32 GetCountdownNumber() const { return CountdownNumber; }
	
	UFUNCTION(BlueprintCallable, Category= "Kart|Countdown")
	void StartCountdown();
	
	
	// course : tours et chrono
	
	UFUNCTION(BlueprintPure, Category= "Kart|Race")
	int32 GetLapCount() const { return LapCount; }
	
	UFUNCTION(BlueprintPure, Category= "Kart|Race")
	int32 GetCheckpointCount() const { return CheckpointCount; }
	
	UFUNCTION(BlueprintPure, Category= "Kart|Race")
	float GetRaceTime() const;
	
	void NotifyKartFinished(AKartPawn* Kart);
	
	UFUNCTION(BlueprintPure, Category= "Kart|CountDown|Debug")
	bool IsDebugTextEnabled() { return bShowDebugText; }
	
	UFUNCTION(BlueprintPure, Category= "Kart|Race")
	static FText FormatRaceTime(float Seconds);
	
protected:
	 
	
	virtual  void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Race", meta = (ClampMin = "1"))
	int32 LapCount = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= " Kart|Countdown")
	bool bUseCountdown = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Countdown", meta = (ClampMin = "1"))
	int32 CountdownSeconds = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Countdown",meta = (ClampMin = "0"))
	float StartDelay = 1.f;
	
	UPROPERTY(EditAnywhere, Category= "Kart|Countdown|Debug")
	bool bShowDebugText = true;
	
	UPROPERTY(EditAnywhere, Category= "Kart|Countdown|Debug")
	float DebugTextScale = 6.f;
	
	UPROPERTY(EditAnywhere, Category= "Kart|Countdown|Debug")
	float GoMessageDuration = 1.f;
	
	UPROPERTY(EditAnywhere, Category= "Kart|Countdown|Debug")
	float RaceDebugTextScale = 2.f;
	
private:
	void BeginCountDown();
	
	void CountdownStep();
	
	void StartRace();
	
	void SetAllKartCanMove(bool bCanMove) const;
	
	void CountCheckpoints();
	
	void ShowRaceDebugText() const;
	
	void ShowDebugText(const  FString& Text, const FColor& Color, float Duration) const;
	
	FTimerHandle CountDownTimer;
	
	int32 CountdownNumber = 0;
	
	bool bRaceStarted = false;
	
	float RaceStartTime = 0.f;
	
	int32 CheckpointCount = 0;
	
};
