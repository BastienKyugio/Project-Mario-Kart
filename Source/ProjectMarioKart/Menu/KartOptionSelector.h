#pragma once

#include "CoreMinimal.h"
#include "Menu/KartOptionRow.h"
#include "KartOptionSelector.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionSelectionChanged, int32, NewIndex);

UCLASS()
class PROJECTMARIOKART_API UKartOptionSelector : public UKartOptionRow
{
	GENERATED_BODY()
	
public:
	void SetChoices(const TArray<FText>& InChoices);
	
	void SetSelectedIndex(int32 Index);
	
	int32 GetSelectedIndex() const { return SelectedIndex; }
	
	UPROPERTY(BlueprintAssignable, Category="Kart|Option")
	FOnOptionSelectionChanged OnSelectionChanged;
	
protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> PreviousButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> NextButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ValueText;
	
private:	
	UFUNCTION()
	void HandlePreviousClicked();
	
	UFUNCTION()
	void HandleNextClicked();
	
	void Step(int32 Direction);
	
	void Refresh();
	
	TArray<FText> Choices;
	
	int32 SelectedIndex;
	
};