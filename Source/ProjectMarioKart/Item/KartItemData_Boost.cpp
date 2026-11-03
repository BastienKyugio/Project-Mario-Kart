#include "Item/KartItemData_Boost.h"

#include  "KartPawn.h"

bool UKartItemData_Boost::Activate(AKartPawn* Kart) const
{
	if (Kart == nullptr)
	{
		return false;
	}
	
	Kart->ApplyBoost(BoostDuration);
	return true;
}

