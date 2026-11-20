#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KartRaceTrack.generated.h"

class USplineComponent;
class UStaticMesh;
class UMaterialInterface;

UCLASS()
class PROJECTMARIOKART_API AKartRaceTrack : public AActor
{
	GENERATED_BODY()
	
public:
	AKartRaceTrack();
	
	virtual void  OnConstruction(const FTransform& Transform) override;
	
	static AKartRaceTrack* Find(const UObject* WorldContext);
	
	float GetTrackLength() const;
	
	float GetDistanceAlongTrack(const FVector& Location) const;
	
	FVector GetPointAtDistances(float Distance, float LateralOffset) const;
	
	float GetLateralOffset(const FVector& Location) const;
	
	FVector GetDirectionAtDistances(float Distance) const;
	
	float GetTurnAngle(float FromDistance, float ToDistance) const;
	
	float GetHalfWidth() const { return TrackWidth * 0.5f; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Kart|Components")
	TObjectPtr<USplineComponent> Spline;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Track", meta=(ClampMin = "200"))
	float TrackWidth = 1400.f;
	
	UPROPERTY(editAnywhere, Category= "Kart|Track")
	TObjectPtr<UStaticMesh> RoadMesh;
	
	UPROPERTY(EditAnywhere, Category="Kart|Track")
	TObjectPtr<UMaterialInterface> RoadMaterial;
	
	UPROPERTY(EditAnywhere, Category="Kart|Track", meta=(ClampMin = "100"))
	float RoadSegmentLength = 500.f;
	
private:
	float WrapDistance(float Distance) const;
};