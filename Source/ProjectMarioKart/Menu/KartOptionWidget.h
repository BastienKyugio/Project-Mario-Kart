#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include  "KartOptionWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetSwitcher;
class UKartOptionSelector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOptionClosed);

UCLASS()
class PROJECTMARIOKART_API UKartOptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void FocusDefaultButton();
	
	UPROPERTY(BlueprintAssignable, Category="Kart|Option")
	FOnOptionClosed OnClosed;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> TabSwitcher;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> GraphicsTabButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UKartOptionSelector> ResolutionSelector;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UKartOptionSelector> WindowModeSelector;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UKartOptionSelector> QualitySelector;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UKartOptionSelector> VSyncSelector;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UKartOptionSelector> FrameRateSelector;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ApplyButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> DefaultButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BackButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidget> ConfirmPannel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ConfirmText;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> KeepButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RevertButton;
	
	UPROPERTY(EditAnywhere, Category="Kart|Option", meta = (ClampMin = "1"))
	float ConfirmDuration = 10.f;
	
private:
	UFUNCTION()
	void HandleGraphicsTabClicked();
	
	void ShowTab(int32 TabIndex);
	
	void SetupGraphicsChoices();
	
	void LoadFromSettings();
	
	UFUNCTION()
	void HandleResolutionChanged(int32 NewIndex);
	
	UFUNCTION()
	void HandleWindowModeChanged(int32 NewIndex);
	
	UFUNCTION()
	void HandleQualityChanged(int32 NewIndex);
	
	UFUNCTION()
	void HandleVSyncChanged(int32 NewIndex);
	
	UFUNCTION()
	void HandleFrameRateChanged(int32 NewIndex);
	
	UFUNCTION()
	void HandleApplyClicked();
	
	UFUNCTION()
	void HandleDefaultClicked();
	
	UFUNCTION()
	void HandleBackClicked();
	
	UFUNCTION()
	void HandleKeepClicked();
	
	UFUNCTION()
	void HandleRevertClicked();
	
	void ShowConfirmPannel(bool bShow);
	
	TArray<FIntPoint> Resolutions;
	
	TArray<float> FrameRatesLimits = {30.f, 60.f, 120.f, 144.f, 0.f };
	
	float ConfirmTimeLeft = 0.f;
	
	
	
};