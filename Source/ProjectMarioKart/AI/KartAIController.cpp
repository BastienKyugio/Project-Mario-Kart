#include "AI/KartAIController.h"

#include "AI/KartDriverProfile.h"
#include "EngineUtils.h"
#include "Item/KartItemComponent.h"
#include "Item/KartItemData_Boost.h"
#include "Item/KartItemData_SpawnActor.h"
#include "Item/KartItemProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "KartPawn.h"
#include "Settings/KartGameUserSettings.h"
#include "Track/KartRaceTrack.h"

AKartAIController::AKartAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	DifficultyLevels.Add({0.92f, 0.6f, -0.3f });
	DifficultyLevels.Add({1.f, 0.5f, 0.f });
	DifficultyLevels.Add({1.04f, 0.4f, 0.2f });  
	
}

void AKartAIController::BeginPlay()
{
	Super::BeginPlay();
	
	Track = AKartRaceTrack::Find(this);
	if (Track == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KartAIController : aucun AKartRaceTrack dans le niveau, l'IA ne sait pas où aller"));
	}
	WanderTime = FMath::FRandRange(0.f, 100.f);
}

void AKartAIController::SetDriverProfile(UKartDriverProfile* InProfile)
{
	Profile = InProfile;
}

const UKartDriverProfile* AKartAIController::GetProfileOrDefault() const
{
	return Profile !=nullptr ? Profile.Get() : GetDefault<UKartDriverProfile>();
}

void AKartAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Kart = Cast<AKartPawn>(GetPawn());
	if (Kart == nullptr || Track == nullptr)
	{
		return;
	}
	
	if (!Kart->CanMove())
	{
		Kart->SetThrottleInput(0.f);
		Kart->SetSteerInput(0.f);
	}
	
	WanderTime += DeltaTime;

	UpdateProgress();
	
	if (UpdateRecovery(DeltaTime))
	{
		return;
	}
	
	UpdateLane(DeltaTime);
	UpdateSteering(DeltaTime);
	UpdateThrottleAndDrift();
	UpdateItem(DeltaTime);
	UpdateRubberBand(DeltaTime);
}

void AKartAIController::UpdateProgress()
{
	TrackDistance = Track->GetDistanceAlongTrack(Kart->GetActorLocation());
	MyProgress= GetTotalProgress(Kart, MyLastDistance, MyLaps);
}

void AKartAIController::UpdateSteering(float DeltaTime)
{
	const UKartDriverProfile* Driver = GetProfileOrDefault();
	
	const float Speed = FMath::Abs(Kart->GetSpeed());
	const float LookAhead = LookAheadMin + Speed * LookAheadPerSpeed;
	const FVector Target = Track->GetPointAtDistances(TrackDistance + LookAhead, CurrentLane);
	
	const FVector Forward = Kart->GetActorForwardVector().GetSafeNormal2D();
	const FVector ToTarget = (Target - Kart->GetActorLocation()).GetSafeNormal2D();
	
	const float Side = FVector::CrossProduct(Forward, ToTarget).Z;
	const float Front = FVector::DotProduct(Forward, ToTarget);
	const float Angle = FMath::RadiansToDegrees(FMath::Atan2(Side, Front));
	
	float DesiredSteer = FMath::Clamp(Angle/ FullSteerAngle, -1.f, 1.f);
	
	const float Precision = FMath::Clamp(Driver->SteeringPrecision + GetDifficulty().PrecisionBonus, 0.f, 1.f);
	DesiredSteer +=FMath::Sin(WanderTime * 2.3f) * (1.f - Precision) * 0.3f;
	
	CurrentSteer = FMath::FInterpTo(CurrentSteer, DesiredSteer, DeltaTime, 1.f / Driver->ReactionTime);
	Kart->SetSteerInput(FMath::Clamp(CurrentSteer, -1.f, 1.f));
}

void AKartAIController::UpdateThrottleAndDrift()
{
	const float Speed = Kart->GetSpeed();
	
	UpComingTurnAngle = Track->GetTurnAngle(TrackDistance, TrackDistance + CornerLookAhead);
	
	float Throttle = 1.f;
	if (UpComingTurnAngle > BrakeTurnAngle && Speed > CornerSpeed)
	{
		Throttle = 0.f;
	}
	
	if (bHoldBack)
	{
		Throttle = FMath::Min(Throttle, 0.7f);
	}
	
	Kart->SetThrottleInput(Throttle);
	
	const bool bGoodCorner = UpComingTurnAngle > DriftMinTurnAngle && UpComingTurnAngle < DriftMaxTurnAngle;
	if (!Kart->IsDrifting())
	{
		if (bGoodCorner && FMath::Abs(CurrentSteer) > 0.5f)
		{
			Kart->StartDrift();
			
			DriftSide = FMath::Sign(CurrentSteer);
		}
	}
	else if (UpComingTurnAngle < DriftMinTurnAngle * 0.5f || CurrentSteer * DriftSide < 0.f)
	{
		Kart->EndDrift();
		DriftSide = 0.f; 
	}
	
}

void AKartAIController::UpdateLane(float DeltaTime)
{
	const UKartDriverProfile* Driver = GetProfileOrDefault();
	const float MaxOffset = Track->GetHalfWidth() * LaneUsage;
	const float Wander = FMath::Sin(WanderTime * 0.35f) * Driver->LaneWander;
	float TargetLane = FMath::Clamp(Driver->PreferredLane + Wander, -1.f, 1.f) * MaxOffset;
	
	bHoldBack = false;
	if (const AKartPawn* Blocker = FindNearestKart(true, OvertakeDistance, 30.f))
	{
		if (Driver->Aggressiveness >= 0.3f)
		{
			const float BlockerLane = Track->GetLateralOffset(Blocker->GetActorLocation());
			const float Side = BlockerLane > 0.f ? -1.f : 1.f;
			TargetLane = FMath::Clamp(BlockerLane + Side * OvertakeOffset, -MaxOffset, MaxOffset);
		}
		else
		{
			bHoldBack = true;
		}
	}
	CurrentLane = FMath::FInterpConstantTo(CurrentLane, TargetLane, DeltaTime, LaneChangeSpeed);
}

AKartPawn* AKartAIController::FindNearestKart(bool bAhead, float MaxDistance, float ConeAngle) const
{
	const FVector MyLocation = Kart->GetActorLocation();
	const FVector LookDirection = (bAhead ? 1.f : -1.f) * Kart->GetActorForwardVector().GetSafeNormal2D();
	
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(ConeAngle));
	
	AKartPawn* Nearest = nullptr;
	float NearestDistance = MaxDistance;
	
	for (TActorIterator<AKartPawn> It(GetWorld()); It; ++It)
	{
		AKartPawn* Other = *It;
		if (Other == Kart)
		{
			continue;
		}
		
		const FVector ToOther = Other->GetActorLocation() - MyLocation;
		const float Distance = ToOther.Size2D();
		if (Distance > NearestDistance)
		{
			continue;
		}
		if (FVector::DotProduct(ToOther.GetSafeNormal2D(), LookDirection) < MinDot)
		{
			continue;
		}
		
		Nearest = Other;
		NearestDistance = Distance;
	}
	return Nearest;
}

bool AKartAIController::UpdateRecovery(float DeltaTime)
{
	if (ReverseTimer > 0.f)
	{
		ReverseTimer -= DeltaTime;
		Kart->SetThrottleInput(-1.f);
		
		Kart->SetSteerInput(-FMath::Sign(CurrentSteer));
		return true;
	}
	
	if (Kart->IsSpinningOut())
	{
		StuckTimer = 0.f;
		return false;
	}
	
	const bool bTooSlow = Kart->GetSpeed() < StuckSpeed;
	const bool bFarFromRoad = FMath::Abs(Track->GetLateralOffset(Kart->GetActorLocation())) > Track->GetHalfWidth() * 2.f;
	
	const FVector RoadPoint = Track->GetPointAtDistances(TrackDistance);
	if (Kart->GetActorLocation().Z < RoadPoint.Z - 1000.f)
	{
		RespawnOnTrack();
		return true;
	}
	
	StuckTimer = bTooSlow ? StuckTimer + DeltaTime : 0.f;
	if (StuckTimer >= StuckTime)
	{
		StuckTimer = 0.f;
		ReverseTimer = ReverseDuration;
	}
	
	RecoveryTimer = (bTooSlow || bFarFromRoad) ? RecoveryTimer + DeltaTime : 0.f;
	if (RecoveryTimer >= RespawnTime)
	{
		RespawnOnTrack();
		return true;
	}
	return ReverseTimer >0.f;
}

void AKartAIController::RespawnOnTrack()
{
	const FVector Location = Track->GetPointAtDistances(TrackDistance) + FVector(0.f, 0.f, 100.f);
	const FRotator Rotation(0.f, Track->GetDirectionAtDistances(TrackDistance).Rotation().Yaw, 0.f);
	
	Kart->SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
	
	StuckTimer = 0.f;
	ReverseTimer = 0.f;
	RecoveryTimer = 0.f;
	CurrentSteer = 0.f;
	CurrentLane = 0.f;
}

void AKartAIController::UpdateItem(float DeltaTime)
{
	UKartItemComponent* Items = Kart->GetItemComponent();
	if (Items == nullptr || !Items->HasItem() || Items->IsRouletteRunning())
	{
		ItemHolderTimer = 0.f;
		return;
	}
	
	ItemHolderTimer += DeltaTime;
	const UKartItemData* Item = Items->GetCurrentItem();
	bool bUse = false;
	
	if (Cast<UKartItemData_Boost>(Item) != nullptr)
	{
		bUse = UpComingTurnAngle < 15.f;
	}
	else if (const UKartItemData_SpawnActor* SpawnItem = Cast<UKartItemData_SpawnActor>(Item))
	{
		const bool bIsProjectile = SpawnItem->ActorClass != nullptr && SpawnItem->ActorClass->IsChildOf(AKartItemProjectile::StaticClass());
		if (bIsProjectile)
		{
			bUse = FindNearestKart(true, ProjectileRange, ProjectileAimAngle) != nullptr;
		}
		else
		{
			bUse = FindNearestKart(false, TrapRange, 45.f) != nullptr;
		}
	}
	if (ItemHolderTimer >= GetProfileOrDefault()->ItemPatience)
	{
		bUse = true;
	}
	if (bUse)
	{
		Kart->UseItem();
		ItemHolderTimer = 0.f;
	}
}

float AKartAIController::GetTotalProgress(const AKartPawn* OtherKart, float& InOutLastDistance, int32& InOutLaps)
{
	const float Length = Track->GetTrackLength();
	const float Distance = Track->GetDistanceAlongTrack(OtherKart->GetActorLocation());
	
	if (InOutLastDistance < 0.f)
	{
		InOutLaps = Distance > Length * 0.5f ? -1 :0;
	}
	else
	{
		const float Delta = Distance - InOutLastDistance;
		if (Delta < - Length *0.5f)
		{
			++InOutLaps;
		}
		else if (Delta > Length * 0.5f)
		{
			--InOutLaps;
		}
	}
	
	InOutLastDistance = Distance;
	return InOutLaps * Length * Distance;
}

void AKartAIController::UpdateRubberBand(float DeltaTime)
{
	const FKartAIDifficulty& Difficulty = GetDifficulty();
	
	const float BaseMultiplier = GetProfileOrDefault()->SpeedFactor * Difficulty.SpeedFactor;
	float Correction = 0.f;
	
	const AKartPawn* PlayerKart = Cast<AKartPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (PlayerKart != nullptr)
	{
		const float Gap = GetTotalProgress(PlayerKart, PlayerLastDistance, PlayerLaps) - MyProgress;
		
		const float Excess = FMath::Max(FMath::Abs(Gap) - RubberBandDeadZone, 0.f);
		const float Ratio = FMath::Clamp(Excess / FMath::Max(RubberBandMaxGap - RubberBandDeadZone, 1.f), 0.f, 1.f);
		
		Correction = FMath::Sign(Gap) * Ratio * RubberBandMaxCorrection * Difficulty.RubberBandStrength;
	}
	
	CurrentSpeedMultiplier = FMath::FInterpTo(CurrentSpeedMultiplier, BaseMultiplier * (1.f + Correction), DeltaTime, 0.5f);
	Kart->SetSpeedMultiplier(CurrentSpeedMultiplier);
}

const FKartAIDifficulty AKartAIController::GetDifficulty() const
{
	static const FKartAIDifficulty Neutral;
	if (DifficultyLevels.Num() == 0)
	{
		return Neutral;
	}
	
	const UKartGameUserSettings* Settings = UKartGameUserSettings::Get();
	const int32 Level = Settings != nullptr ? Settings->GetDifficulty() : 1;
	return DifficultyLevels[FMath::Clamp(Level, 0, DifficultyLevels.Num() - 1)];
}
