#include "Menu/KartMainMenuWidget.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Menu/KartOptionWidget.h"

void UKartMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayButton->OnClicked.AddDynamic(this, &UKartMainMenuWidget::HandlePlayClicked);
	OptionsButton->OnClicked.AddDynamic(this, &UKartMainMenuWidget::HandleOptionsClicked);
	QuitButton->OnClicked.AddDynamic(this, &UKartMainMenuWidget::HandleQuitClicked);
}

void UKartMainMenuWidget::FocusDefaultButton()
{
	PlayButton->SetUserFocus(GetOwningPlayer());
}

void UKartMainMenuWidget::HandlePlayClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, RaceLevel);
}

void UKartMainMenuWidget::HandleOptionsClicked()
{
	if (OptionsWidgetClass == nullptr)
	{
		return;
	}
	OptionsWidget = CreateWidget<UKartOptionWidget>(GetOwningPlayer(), OptionsWidgetClass);
	if (OptionsWidget == nullptr)
	{
		return;
	}
	
	OptionsWidget->OnClosed.AddDynamic(this, &UKartMainMenuWidget::HandleOptionsClosed);
	OptionsWidget->AddToViewport();
	OptionsWidget->FocusDefaultButton();
	
	SetVisibility(ESlateVisibility::Collapsed);
	
}


void UKartMainMenuWidget::HandleQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UKartMainMenuWidget::HandleOptionsClosed()
{
	OptionsWidgetClass = nullptr;
	SetVisibility(ESlateVisibility::Visible);
	FocusDefaultButton();
}
