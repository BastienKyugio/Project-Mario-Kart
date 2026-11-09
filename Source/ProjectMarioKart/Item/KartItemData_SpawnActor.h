#pragma once

#include "CoreMinimal.h"
#include "Item/KartItemData.h"
#include "KartItemData_SpawnActor.generated.h"

UCLASS()
class PROJECTMARIOKART_API UKartItemData_SpawnActor : public UKartItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintCallable, Category = "Item|Spawn")
	TSubclassOf<AActor> ActorClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintCallable, Category = "Item|Spawn")
	FVector SpawnOffset = FVector(-150.f, 0.f, 0.f);
	
	virtual bool Activate(AKartPawn* Kart) const override;
	
};
