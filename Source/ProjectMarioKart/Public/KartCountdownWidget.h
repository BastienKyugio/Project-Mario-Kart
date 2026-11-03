// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KartCountdownWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTMARIOKART_API UKartCountdownWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Kart|Countdown")
	FText GoText = NSLOCTEXT("Kart", "Go", "GO!");
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Kart|Countdown", meta = (ClampMin = "0"))
	float GoDisplayTime = 1.f;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Kart|Countdown")
	void OnCountdownNumberShow(int32 Number);
	
private:
	UFUNCTION()
	void HandleCountDownTick(int32 Number);
	
	void HideCountdown();
	
	FTimerHandle HideTimer;
};
