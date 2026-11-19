#include "Menu/KartMenuPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Menu/KartMainMenuWidget.h"
#include "Settings/KartGameUserSettings.h"

void AKartMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController())
	{
		return;
	}
	
	if (const UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->ApplyAudioSettings(this);
	}
	
	if (MainMenuWidgetClass == nullptr)
	{
		return;
	}
	
	MainMenuWidget = CreateWidget<UKartMainMenuWidget>(this, MainMenuWidgetClass);
	if (MainMenuWidget == nullptr)
	{
		return;
	}
	MainMenuWidget->AddToViewport();
	
	bShowMouseCursor = true;
	
	MainMenuWidget->FocusDefaultButton();
	
}
