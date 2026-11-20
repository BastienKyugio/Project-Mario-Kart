#include "KartPauseMenuWidget.h"

#include "Components/Button.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "KartPlayerController.h"
#include "Menu/KartOptionWidget.h"

void UKartPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ResumeButton->OnClicked.AddDynamic(this, &UKartPauseMenuWidget::HandleResumeClicked);
	OptionsButton->OnClicked.AddDynamic(this, &UKartPauseMenuWidget::HandleOptionsClicked);
	RestartButton->OnClicked.AddDynamic(this, &UKartPauseMenuWidget::HandleRestartClicked);
	MainMenuButton->OnClicked.AddDynamic(this, &UKartPauseMenuWidget::HandleMainMenuClicked);
}

void UKartPauseMenuWidget::FocusDefaultButton()
{
	ResumeButton->SetUserFocus(GetOwningPlayer());
}

FReply UKartPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Right)
	{
		HandleResumeClicked();
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(InGeometry,InKeyEvent);
}

void UKartPauseMenuWidget::HandleResumeClicked()
{
	if (AKartPlayerController* PlayerController = GetOwningPlayer<AKartPlayerController>())
	{
		PlayerController->ResumeGame();
	}
}

void UKartPauseMenuWidget::HandleOptionsClicked()
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
	
	OptionsWidget->OnClosed.AddDynamic(this, &UKartPauseMenuWidget::HandleOptionsClosed);
	OptionsWidget->AddToViewport();
	OptionsWidget->FocusDefaultButton();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UKartPauseMenuWidget::HandleOptionsClosed()
{
	OptionsWidget = nullptr;
	SetVisibility(ESlateVisibility::Visible);
	FocusDefaultButton();
}

void UKartPauseMenuWidget::HandleRestartClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	
	UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(this)));
}

void UKartPauseMenuWidget::HandleMainMenuClicked()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
}