#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KartItemComponent.generated.h"

class UKartItemData;
class UKartItemPoolData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemChanged, UKartItemData*, Item, int32, UsesLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRouletteStarted, UKartItemPoolData*, Pool, float, Duration);

UCLASS(ClassGroup = (Kart), meta = (BlueprintSpawnableComponent))
class PROJECTMARIOKART_API UKartItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKartItemComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Kart|Items")
	bool TryGiveRandomItem(UKartItemPoolData* Pool);
	
	UFUNCTION(BlueprintCallable, Category = "Kart|Items")
	void UseItem();
	
	UFUNCTION(BlueprintCallable, Category = "Kart|Items")
	void ClearItems();
	
	UFUNCTION(BlueprintPure, Category = "Kart|Items")
	bool HasItem() const {return CurrentItem != nullptr; }
	
	UFUNCTION(BlueprintPure, Category = "Kart|Items")
	bool IsRouletteRunning() const { return bRouletteRunning; }
	
	UFUNCTION(BlueprintPure, Category = "Kart|Items")
	UKartItemData* GetCurrentItem() const {return CurrentItem; }
	
	UFUNCTION(BlueprintPure, Category = "Kart|Items")
	int32 GetUsesLeft() const {return UsesLeft; }
	
	UPROPERTY(BlueprintAssignable, Category = "Kart|Items")
	FOnItemChanged OnItemChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Kart|Items")
	FOnRouletteStarted OnRouletteStarted;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Items", meta = (ClampMin = "0"))
	float RouletteDuration = 2.f;
	
	UPROPERTY(EditAnywhere, Category = "Kart|Items|Debug")
	bool bShowDebugText = true;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Kart|Items")
	TObjectPtr<UKartItemData> CurrentItem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Items")
	int32 UsesLeft;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Items")
	bool bRouletteRunning = false;
	
private:
	void FinishRoulette();
	
	void SetItem(UKartItemData* NewItem, int32 NewUses);
	
	UPROPERTY()
	TObjectPtr<UKartItemData> PendingTime;
	
	FTimerHandle RouletteTimer;
};
