#include "Menu/KartOptionSlider.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UKartOptionSlider::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ValueSlider->OnValueChanged.AddDynamic(this, &UKartOptionSlider::HandleSliderValueChanged);
}

void UKartOptionSlider::SetRange(float MinValue, float MaxValue, float StepSize)
{
	ValueSlider->SetMinValue(MinValue);
	ValueSlider->SetMaxValue(MaxValue);
	
	ValueSlider->SetStepSize(StepSize);
}

void UKartOptionSlider::SetValue(float Value)
{
	ValueSlider->SetValue(Value);
	RefreshText();
}

float UKartOptionSlider::GetValue() const
{
	return ValueSlider->GetValue();
}

void UKartOptionSlider::HandleSliderValueChanged(float Value)
{
	RefreshText();
	OnValueChanged.Broadcast(Value);
}

void UKartOptionSlider::RefreshText()
{
	const float Value = ValueSlider->GetValue();
	
	const int32 Shown = bShowAsPercent ? FMath::RoundToInt(Value * 100.f) : FMath::RoundToInt(Value);
	ValueText->SetText(FText::FromString(bShowAsPercent ? FString::Printf(TEXT("%d %%"), Shown) : FString::FromInt(Shown)));
}