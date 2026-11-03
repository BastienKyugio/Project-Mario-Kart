#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KartItemPoolData.generated.h"

class UKartItemData;

USTRUCT(BlueprintType)
struct FKartItemPoolEntry
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UKartItemData> Item;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0"))
	float Weight = 1.f;
	
};

UCLASS(BlueprintType)
class PROJECTMARIOKART_API UKartItemPoolData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TArray<FKartItemPoolEntry> Entries;
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	UKartItemData* PickRandomItem() const;
	
};
