#include "KartOptionSelector.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UKartOptionSelector::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PreviousButton->OnClicked.AddDynamic(this, &UKartOptionSelector::HandlePreviousClicked);
	NextButton->OnClicked.AddDynamic(this, &UKartOptionSelector::HandleNextClicked);
}

void UKartOptionSelector::SetChoices(const TArray<FText>& InChoices)
{
	Choices = InChoices;
	SelectedIndex = 0;
	Refresh();
}

void UKartOptionSelector::SetSelectedIndex(int32 Index)
{
	SelectedIndex = Choices.IsEmpty() ? 0 : FMath::Clamp(Index, 0, Choices.Num() - 1);
	Refresh();
}

void UKartOptionSelector::HandlePreviousClicked()
{
	Step(-1);
}
void UKartOptionSelector::HandleNextClicked()
{
	Step(1);
}

void UKartOptionSelector::Step(int32 Direction)
{
	if (Choices.IsEmpty())
	{
		return;
	}
	
	SelectedIndex = (SelectedIndex + Direction + Choices.Num()) % Choices.Num();
	Refresh();
	
	OnSelectionChanged.Broadcast(SelectedIndex);
}

void UKartOptionSelector::Refresh()
{
	ValueText->SetText(Choices.IsValidIndex(SelectedIndex) ? Choices[SelectedIndex] : FText::GetEmpty());
}
