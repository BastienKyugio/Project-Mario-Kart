#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KartDriverProfile.generated.h"

UCLASS(BlueprintType)
class PROJECTMARIOKART_API UKartDriverProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Driver")
	FText DriverName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Driver|Trajectoire", meta = (ClampMin = "-1", ClampMax ="1"))
	float PreferredLane = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Driver|Trajectoire", meta = (ClampMin = "-1", ClampMax = "1"))
	float LaneWander = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Driver|Pilotage", meta = (ClampMin = "0", ClampMax = "1"))
	float Aggressiveness = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Driver|Pilotage", meta = (ClampMin = "0", ClampMax = "1"))
	float SteeringPrecision = 0.8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Driver|Pilotage", meta = (ClampMin = "0.05"))
	float ReactionTime = 0.15f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Driver|Pilotage", meta = (ClampMin = "0.8", ClampMax = "1.2"))
	float SpeedFactor = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Driver|Objets", meta = (ClampMin = "8"))
	float ItemPatience = 4.f;
	
};