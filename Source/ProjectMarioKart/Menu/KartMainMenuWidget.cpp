#include "Menu/KartMainMenuWidget.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UKartMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	PlayButton->OnClicked.AddDynamic(this, &UKartMainMenuWidget::HandlePlayClicked);
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

void UKartMainMenuWidget::HandleQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}