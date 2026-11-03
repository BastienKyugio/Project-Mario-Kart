// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KartRaceHudWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class PROJECTMARIOKART_API UKartRaceHudWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LapText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Race")
	FText FinishedText = NSLOCTEXT("Kart", "Finished", "Fini !");
	
private:
	UFUNCTION()
	void HandleRaceStarted();
	
};
