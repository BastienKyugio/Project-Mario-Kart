#include "KartItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Item/KartItemComponent.h"
#include "Item/KartItemData.h"
#include "Item/KartItemPoolData.h"

void UKartItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ShowIcon(nullptr);
	UsesText->SetText(FText::GetEmpty());
	
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &UKartItemWidget::HandlePossessedPawnChanged);
		BindToPawn(PlayerController->GetPawn());
	}
}

void UKartItemWidget::NativeDestruct()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &UKartItemWidget::HandlePossessedPawnChanged);
	}
	BindToPawn(nullptr);
	
	Super::NativeDestruct();
}

void UKartItemWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BindToPawn(NewPawn);
}

void UKartItemWidget::BindToPawn(APawn* Pawn)
{
	if (ItemComponent.IsValid())
	{
		ItemComponent->OnItemChanged.RemoveDynamic(this, &UKartItemWidget::HandleItemChanged);
		ItemComponent->OnRouletteStarted.RemoveDynamic(this, &UKartItemWidget::HandleRouletteStarted);
	}
	
	ItemComponent = Pawn != nullptr ? Pawn->FindComponentByClass<UKartItemComponent>() : nullptr;
	RoulettePool = nullptr;
	
	if (!ItemComponent.IsValid())
	{
		return;
	}
	
	ItemComponent->OnItemChanged.AddUniqueDynamic(this, &UKartItemWidget::HandleItemChanged);
	ItemComponent->OnRouletteStarted.AddDynamic(this, &UKartItemWidget::HandleRouletteStarted);
	HandleItemChanged(ItemComponent->GetCurrentItem(), ItemComponent->GetUsesLeft());
}

void UKartItemWidget::HandleRouletteStarted(UKartItemPoolData* Pool, float Duration)
{
	RoulettePool = Pool;
	RouletteIndex = 0;
	RouletteTimer = RouletteIconInterval;
	UsesText->SetText(FText::GetEmpty());
}

void UKartItemWidget::HandleItemChanged(UKartItemData* Item, int32 UsesLeft)
{
	RoulettePool = nullptr;
	ShowIcon(Item != nullptr ? Item->Icon.Get() : nullptr);
	
	if (Item != nullptr && UsesLeft > 1)
	{
		UsesText->SetText(FText::Format(NSLOCTEXT("Kart", "ItemUses", "x{0]"), FText::AsNumber(UsesLeft)));
	}
	else
	{
		UsesText->SetText(FText::GetEmpty());
	}
}

void UKartItemWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (RoulettePool == nullptr || RoulettePool->Entries.Num() == 0)
	{
		return;
	}
	
	RouletteTimer += InDeltaTime;
	
	if (RouletteTimer < RouletteIconInterval)
	{
		return;
	}
	
	RouletteTimer -= RouletteIconInterval;
	
	RouletteIndex = (RouletteIndex + 1) % RoulettePool->Entries.Num();
	
	const UKartItemData* ShowItem = RoulettePool->Entries[RouletteIndex].Item;
	ShowIcon(ShowItem != nullptr ? ShowItem->Icon.Get() : nullptr);
}

void UKartItemWidget::ShowIcon(UTexture2D* Texture)
{
	if (Texture == nullptr)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	ItemIcon->SetBrushFromTexture(Texture);
	ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
}
