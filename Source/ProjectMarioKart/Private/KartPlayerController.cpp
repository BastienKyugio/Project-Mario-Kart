#include "KartPlayerController.h"

#include "KartCountdownWidget.h"
#include "KartRaceHudWidget.h"

void AKartPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// L'UI n'existe que pour le joueur qui joue sur cette machinex
	if (!IsLocalController())
	{
		return;
	}

	if (CountdownWidgetClass != nullptr)
	{
		CountdownWidget = CreateWidget<UKartCountdownWidget>(this, CountdownWidgetClass);
		if (CountdownWidget != nullptr)
		{
			CountdownWidget->AddToViewport();
		}
	}

	if (RaceHudWidgetClass != nullptr)
	{
		RaceHudWidget = CreateWidget<UKartRaceHudWidget>(this, RaceHudWidgetClass);
		if (RaceHudWidget != nullptr)
		{
			RaceHudWidget->AddToViewport();
		}
	}
}
 