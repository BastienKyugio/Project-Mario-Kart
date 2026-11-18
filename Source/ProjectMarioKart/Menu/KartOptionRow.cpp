#include "Menu/KartOptionRow.h"

#include "Components/TextBlock.h"

void UKartOptionRow::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Label != nullptr)
	{
		Label->SetText(LabelText);
	}
}
