#pragma once

#include "CoreMinimal.h"
#include "Item/KartItemData.h"
#include "KartItemData_Boost.generated.h"

UCLASS()
class PROJECTMARIOKART_API UKartItemData_Boost : public UKartItemData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Boost", meta = (ClampMin = "0.1"))
	float BoostDuration = 1.5f;
	
	virtual bool Activate(AKartPawn* Kart) const override;
};
