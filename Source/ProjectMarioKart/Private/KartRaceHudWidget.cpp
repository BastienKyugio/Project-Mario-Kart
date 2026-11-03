#include "KartRaceHudWidget.h"

#include "Components/TextBlock.h"
#include "KartGameMode.h"
#include "KartPawn.h"


void  UKartRaceHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	
	if (AKartGameMode* GameMode = GetWorld()->GetAuthGameMode<AKartGameMode>())
	{
		GameMode->OnRaceStarted.AddUniqueDynamic(this, &UKartRaceHudWidget::HandleRaceStarted);
		
		if (GameMode->IsRaceStarted())
		{
			HandleRaceStarted();
		}
	}
}

void UKartRaceHudWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		if (AKartGameMode* GameMode = GetWorld()->GetAuthGameMode<AKartGameMode>())
		{
			GameMode->OnRaceStarted.RemoveDynamic(this, &UKartRaceHudWidget::HandleRaceStarted);
		}
	}
		
	Super::NativeDestruct();
}

void UKartRaceHudWidget::HandleRaceStarted()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UKartRaceHudWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const AKartGameMode* GameMode = GetWorld()->GetAuthGameMode<AKartGameMode>();
	
	const AKartPawn* Kart = GetOwningPlayerPawn<AKartPawn>();
	
	if (GameMode == nullptr || Kart == nullptr)
	{
		return;
	}
	
	if (!GameMode->IsRaceStarted())
	{
		TimerText->SetText(AKartGameMode::FormatRaceTime(Kart->GetFinishTime()));
		LapText->SetText(FinishedText);
	}
	else
	{
		TimerText->SetText(AKartGameMode::FormatRaceTime(GameMode->GetRaceTime()));
		
		LapText->SetText(FText::Format(NSLOCTEXT("Kart", "Lap", "Tour {0}/{1}"), FText::AsNumber(Kart->GetCurrentLap()), FText::AsNumber(GameMode->GetLapCount())));
	}
	
}
