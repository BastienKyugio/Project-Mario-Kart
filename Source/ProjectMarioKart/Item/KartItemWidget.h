#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KartItemWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;
class APawn;
class UKartItemComponent;
class UKartItemData;
class UKartItemPoolData;

UCLASS()
class PROJECTMARIOKART_API UKartItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UsesText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Items", meta = (ClampMin = "0.01"))
	float RouletteIconInterval =0.08f;
	
private:
	
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void HandleItemChanged(UKartItemData* Item, int32 UsesLeft);
	
	UFUNCTION()
	void HandleRouletteStarted(UKartItemPoolData* Pool, float Duration);
	
	void BindToPawn(APawn* Pawn);
	
	void ShowIcon(UTexture2D* Texture);
	
	TWeakObjectPtr<UKartItemComponent> ItemComponent;
	
	UPROPERTY()
	TObjectPtr<UKartItemPoolData> RoulettePool;
	
	int32 RouletteIndex = 0;
	
	float RouletteTimer = 0.f;
	
};