// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "KartPawn.h"
//#include "KartPlayerController.h"
//#include "LapZone.h"
#include "TimerManager.h"
#include "KartGameMode.h"


AKartGameMode::AKartGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKartGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	//CountCheckpoints();
	
	if (bUseCountdown)
	{
		StartCountdown();
	}
	else
	{
		bRaceStarted = true;
		RaceStartTime = GetWorld()->GetTimeSeconds();
	}
}

void AKartGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bShowDebugText)
	{
		ShowRaceDebugText();
	}
}

void AKartGameMode::StartCountdown()
{
	bRaceStarted = false;
	CountdownNumber = 0;
	SetAllKartCanMove(false);
	
	for (TActorIterator<AKartPawn> It(GetWorld()); It; ++It)
	{
		It->ResetRaceProgress();
	}
	
	if (StartDelay >0.f)
	{
		GetWorldTimerManager().SetTimer(CountDownTimer, this, &AKartGameMode::BeginCountDown, StartDelay, false);
		
	}
	else
	{
		BeginCountDown();
	}
}
void AKartGameMode::BeginCountDown()
{
	CountdownNumber = CountdownSeconds;
	ShowDebugText(FString::FromInt(CountdownNumber), FColor::Red, 1.f);
	OnCountdownTick.Broadcast(CountdownNumber);
	
	GetWorldTimerManager().SetTimer(CountDownTimer, this, &AKartGameMode::CountdownStep, 1.f, true);
}

void AKartGameMode::CountdownStep()
{
	--CountdownNumber;
	
	if (CountdownNumber <= 0)
	{
		StartRace();
		return;
	}
	
	ShowDebugText(FString::FromInt(CountdownNumber), FColor::Red, 1.f);
	OnCountdownTick.Broadcast(CountdownNumber);
}

void AKartGameMode::StartRace()
{
	GetWorldTimerManager().ClearTimer(CountDownTimer);
	
	CountdownNumber = 0.f;
	bRaceStarted = true;
	SetAllKartCanMove(true);
	
	RaceStartTime = GetWorld()->GetTimeSeconds();
	
	ShowDebugText(TEXT("GO!"), FColor::Green, GoMessageDuration);
	OnCountdownTick.Broadcast(0);
	OnRaceStarted.Broadcast();
}

float AKartGameMode::GetRaceTime() const
{
	if (!IsRaceStarted())
	{
		return 0.f;
	}
	
	return GetWorld()->GetTimeSeconds() - RaceStartTime;
}

void AKartGameMode::NotifyKartFinished(AKartPawn* Kart)
{
	Kart->SetCanMove(false);
	
	OnKartFinished.Broadcast(Kart);
}

FText AKartGameMode::FormatRaceTime(float Seconds)
{
	Seconds = FMath::Max(Seconds, 0.f);
	
	const int32 Minutes = FMath::FloorToInt(Seconds / 60.f);
	
	const float SecondsInMinute = FMath::Fmod(Seconds, 60.f);
	
	const int32 WholeSeconds = FMath::FloorToInt(SecondsInMinute);
	const int32 Milliseconds = FMath::FloorToInt((SecondsInMinute - WholeSeconds) * 1000.f);
	
	return FText::FromString(FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, WholeSeconds, Milliseconds ));
}

void AKartGameMode::SetAllKartCanMove(bool bCanMove) const
{
	for (TActorIterator<AKartPawn> It(GetWorld()); It; ++It)
	{
		It->SetCanMove(bCanMove);
	}
}

void AKartGameMode::ShowRaceDebugText() const
{
	if (GEngine == nullptr || !IsRaceStarted())
	{
		return;
	}
	
	const AKartPawn* Kart = Cast<AKartPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (Kart != nullptr)
	{
		return;
	}
	
	FString Text;
	if (Kart->HasFinishedRace())
	{
		Text = FString::Printf(TEXT("FINI !  %s"), *FormatRaceTime(Kart->GetFinishTime()).ToString());
	}
	else
	{
		Text = FString::Printf(TEXT("Tour %d/%d	%s"), Kart->GetCurrentLap(), LapCount, *FormatRaceTime(GetRaceTime()).ToString());
	}
	
	GEngine->AddOnScreenDebugMessage(101, 0.f, FColor::White, Text, true, FVector2D(RaceDebugTextScale, RaceDebugTextScale));
}

void AKartGameMode:: ShowDebugText(const FString& Text, const FColor& Color, float Duration) const
{
	if (!bShowDebugText || GEngine == nullptr)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(100, Duration, Color, Text, true, FVector2D(DebugTextScale, DebugTextScale));
}
