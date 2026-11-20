#include "KartRaceTrack.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "EngineUtils.h"

AKartRaceTrack::AKartRaceTrack()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;
	
	Spline->SetClosedLoop(true);
}

AKartRaceTrack* AKartRaceTrack::Find(const UObject* WorldContext)
{
	const UWorld* World = WorldContext != nullptr ? WorldContext->GetWorld() : nullptr;
	if (World == nullptr)
	{
		return nullptr;
	}
	
	for (TActorIterator<AKartRaceTrack> It(World); It; ++It)
	{
		return *It;
	}
	return nullptr;
}

void AKartRaceTrack::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (RoadMesh == nullptr)
	{
		return;
	}
	const float Length = GetTrackLength();
	const int32 SegmentCount = FMath::Max(1, FMath::RoundToInt(Length/RoadSegmentLength));
	const float Step = Length / SegmentCount;
	
	const FVector2D WidthScale(TrackWidth/100.f, 1.f);
	
	for (int32 i = 0; i < SegmentCount; ++i)
	{
		USplineMeshComponent* Segment = NewObject<USplineComponent>(this);
		
		Segment->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		Segment->SetStaticMesh(RoadMesh);
		if (RoadMaterial != nullptr)
		{
			Segment->SetMaterial(0, RoadMaterial);
		}
		Segment->SetMobility(EComponentMobility::Movable);
		Segment->SetupAttachment(Spline);
		Segment->RegisterComponent();
		
		const float StartDistance = i * Step;
		const float EndDistance = (i + 1) * Step;
		const FVector StartPos = Spline->GetLocationAtDistanceAlongSpline(StartDistance, ESplineCoordinateSpace::Local);
		const FVector EndPos = Spline->GetLocationAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local);
		const FVector StartTangent = Spline->GetLocationAtDistanceAlongSpline(StartDistance,ESplineCoordinateSpace::Local).GetClampedToMaxSize(Step);
		const FVector EndTangent = Spline->GetTangentAtDistanceAlongSpline(EndDistance, ESplineCoordinateSpace::Local).GetClampedToMaxSize(Step);
		Segment->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
		Segment->SetStartScale(WidthScale);
		Segment->SetEndScale(WidthScale);
		
		Segment->SetCollisionProfileName(TEXT("BlockALl"));
	}
}

float AKartRaceTrack::GetTrackLength() const
{
	return Spline->GetSplineLength();
}

float AKartRaceTrack::WrapDistance(float Distance) const
{
	const float Length = GetTrackLength();
	if (Length <= 0.f)
	{
		return 0.f;
	}
	
	const float Wrapped = FMath::Fmod(Distance, Length);
	return Wrapped < 0.f ? Wrapped + Length : Wrapped;
}

float AKartRaceTrack::GetDistanceAlongTrack(const FVector& Location) const
{
	const float Key = Spline->FindInputKeyClosestToWorldLocation(Location);
	return Spline->GetDistanceAlongSplineAtSplineInputKey(Key);
}

FVector AKartRaceTrack::GetPointAtDistances(float Distance, float LateralOffset) const
{
	const float Wrapped = WrapDistance(Distance);
	const FVector Center = Spline->GetLocationAtDistanceAlongSpline(Wrapped, ESplineCoordinateSpace::World);
	
	const FVector Right = Spline ->GetRightVectorAtDistanceAlongSpline(Wrapped, ESplineCoordinateSpace::World);
	return Center + Right * LateralOffset;
}

float AKartRaceTrack::GetLateralOffset(const FVector& Location) const
{
	const float Distance = GetDistanceAlongTrack(Location);
	const FVector Center = Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	const FVector Right = Spline->GetRightVectorAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

	return FVector::DotProduct(Location - Center, Right);
}

FVector AKartRaceTrack::GetDirectionAtDistances(float Distance) const
{
	return Spline->GetDirectionAtDistanceAlongSpline(WrapDistance(Distance), ESplineCoordinateSpace::World);
}

float AKartRaceTrack::GetTurnAngle(float FromDistance, float ToDistance) const
{
	const FVector From = GetDirectionAtDistances(FromDistance).GetSafeNormal2D();
	const FVector To = GetDirectionAtDistances(ToDistance).GetSafeNormal2D();
	
	const float Cos = FMath::Clamp(FVector::DotProduct(From, To), -1.f, 1.f);
	return FMath::RadiansToDegrees(FMath::Acos(Cos));
}
