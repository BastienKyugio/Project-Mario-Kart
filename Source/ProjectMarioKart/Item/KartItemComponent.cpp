#include "KartItemComponent.h"

#include "Engine/Engine.h"
#include "Item/KartItemData.h"
#include "Item/KartItemPoolData.h"
#include "KartPawn.h"
#include "TimerManager.h"

UKartItemComponent::UKartItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UKartItemComponent::TryGiveRandomItem(UKartItemPoolData* Pool)
{
	if (Pool == nullptr || HasItem() || bRouletteRunning)
	{
		return false;
	}
	
	PendingTime = Pool->PickRandomItem();
	if (PendingTime == nullptr)
	{
		return false;
	}
	
	bRouletteRunning = true;
	OnRouletteStarted.Broadcast(Pool, RouletteDuration);
	
	if (RouletteDuration >0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(RouletteTimer, this, &UKartItemComponent::FinishRoulette, RouletteDuration, false);
	}
	else
	{
		FinishRoulette();
	}
	
	return true;
}

void UKartItemComponent::FinishRoulette()
{
	bRouletteRunning = false;
	
	if (PendingTime != nullptr)
	{
		SetItem(PendingTime, PendingTime->Uses);
		PendingTime = nullptr;
	}
}

void UKartItemComponent::UseItem()
{
	if (bRouletteRunning ||CurrentItem == nullptr)
	{
		return;
	}
	
	AKartPawn* Kart = Cast<AKartPawn>(GetOwner());
	
	if (!CurrentItem ->Activate(Kart))
	{
		return;
	}
	
	if (UsesLeft >1)
	{
		SetItem(CurrentItem, UsesLeft - 1);
	}
	else
	{
		SetItem(nullptr, 0);
	}
}

void UKartItemComponent::ClearItems()
{
	GetWorld()->GetTimerManager().ClearTimer(RouletteTimer);
	bRouletteRunning = false;
	PendingTime = nullptr;
	SetItem(nullptr, 0);
}

void UKartItemComponent::SetItem(UKartItemData* NewItem, int32 NewUses)
{
	CurrentItem = NewItem;
	UsesLeft = NewUses;
	
	OnItemChanged.Broadcast(CurrentItem, UsesLeft );
	
	if ( bShowDebugText && GEngine != nullptr )
	{
		const FString ItemName = CurrentItem != nullptr ? CurrentItem->DisplayName.ToString() : TEXT("aucun");
		GEngine->AddOnScreenDebugMessage(10, 2.f, FColor::Green, FString::Printf(TEXT("Objet : %s x%d"), *ItemName, UsesLeft ));
	}
}