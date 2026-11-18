#pragma once

#include "CoreMinimal.h"
#include "Menu/KartOptionRow.h"
#include "KartOptionSlider.generated.h"

class USlider;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionValueChanged, float, NewValue);

UCLASS()
class PROJECTMARIOKART_API UKartOptionSlider : public UKartOptionRow
{
	GENERATED_BODY()
	
public:
	void SetRange(float MinValue, float MaxValue, float StepSize);
	
	void SetValue(float Value);
	
	float GetValue() const;
	
	UPROPERTY(BlueprintAssignable, Category="Kart|Option")
	FOnOptionValueChanged OnValueChanged;
	
protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> ValueSlider;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ValueText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Kart|Option")
	bool bShowAsPercent = false;
	
private:
	UFUNCTION()
	void HandleSliderValueChanged(float Value);
	
	void RefreshText();
	
	
};