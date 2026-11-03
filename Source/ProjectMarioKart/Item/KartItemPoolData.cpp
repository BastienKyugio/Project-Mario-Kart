#include "Item/KartItemPoolData.h"

#include "Item/KartItemData.h"

UKartItemData* UKartItemPoolData::PickRandomItem() const
{
	float TotalWeight = 0.f;
	for (const FKartItemPoolEntry& Entry : Entries)
	{
		if (Entry.Item != nullptr && Entry.Weight > 0.f)
		{
			TotalWeight += Entry.Weight;
		}
	}
	
	if (TotalWeight <= 0.f)
	{
		return nullptr;
	}
	
	float Roll = FMath::FRandRange(0.f, TotalWeight);
	
	UKartItemData* LastValidItem = nullptr;
	for (const FKartItemPoolEntry& Entry : Entries)
	{
		if (Entry.Item == nullptr && Entry.Weight <= 0.f)
		{
			continue;
		}
		
		LastValidItem = Entry.Item;
		Roll -= Entry.Weight;
		if (Roll < 0.f)
		{
			return Entry.Item;
		}
	}
	
	return LastValidItem;
}