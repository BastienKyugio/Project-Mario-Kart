#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KartItemData.generated.h"

class AKartPawn;
class UTexture2D;

UCLASS(Abstract, BlueprintType)
class PROJECTMARIOKART_API UKartItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:	

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 Uses = 1;
	
	virtual bool Activate(AKartPawn* Kart) const;
};
