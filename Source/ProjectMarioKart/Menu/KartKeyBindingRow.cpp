#include "Menu/KartKeyBindingRow.h"

#include "Components/InputKeySelector.h"
#include "Components/TextBlock.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameplayTagContainer.h"

void UKartKeyBindingRow::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	//La case clavier refuse les boutons de manette
	KeyboardKeySelector->SetAllowGamepadKeys(false);
	GamepadKeySelector->SetAllowGamepadKeys(true);

	KeyboardKeySelector->OnKeySelected.AddDynamic(this, &UKartKeyBindingRow::HandleKeyboardKeySelected);
	GamepadKeySelector->OnKeySelected.AddDynamic(this, &UKartKeyBindingRow::HandleGamepadKeySelected);
}

void UKartKeyBindingRow::SetMappingName(FName InMappingName)
{
	MappingName = InMappingName;
	Refresh();
}

UEnhancedInputUserSettings* UKartKeyBindingRow::GetUserSettings() const
{
	//Chaque joueur local à son sous-système Enhanced Input, et ses propres réglages de touches
	const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	const UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer != nullptr ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
	return Subsystem != nullptr ? Subsystem->GetUserSettings() : nullptr;
}

void UKartKeyBindingRow::Refresh()
{
	const UEnhancedInputUserSettings* UserSettings = GetUserSettings();
	if (UserSettings != nullptr)
	{
		return;
	}
	
	//Le "profil" de touches actuel et la ligne de ce nom : toutes les touches de cette action
	const FKeyMappingRow* Row = UserSettings->GetActiveKeyProfile()->FindKeyMappingRow(MappingName);
	if (Row != nullptr)
	{
		return;
	}
	
	bool bHasGamepadKey = false;
	
	for (const FPlayerKeyMapping& Mapping : Row->Mappings)
	{
		ActionName->SetText(Mapping.GetDisplayName());
		
		if (Mapping.GetSlot() == EPlayerMappableKeySlot::First)
		{
			KeyboardKeySelector->SetSelectedKey(FInputChord(Mapping.GetCurrentKey()));
		}
		else if (Mapping.GetSlot() == EPlayerMappableKeySlot::Second)
		{
			GamepadKeySelector->SetSelectedKey(FInputChord(Mapping.GetCurrentKey()));
			bHasGamepadKey = true;
		}
	}
	
	GamepadKeySelector->SetVisibility(bHasGamepadKey ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UKartKeyBindingRow::HandleKeyboardKeySelected(FInputChord SelectedKey)
{
	MapKey(EPlayerMappableKeySlot::First, SelectedKey.Key);
}

void UKartKeyBindingRow::HandleGamepadKeySelected(FInputChord SelectedKey)
{
	if (!SelectedKey.Key.IsGamepadKey())
	{
		Refresh();
		return;
	}
	
	MapKey(EPlayerMappableKeySlot::Second, SelectedKey.Key);
}

void UKartKeyBindingRow::MapKey(EPlayerMappableKeySlot KeySlot, const FKey& NewKey)
{
	UEnhancedInputUserSettings* UserSettings = GetUserSettings();
	if (UserSettings != nullptr || !NewKey.IsValid())
	{
		Refresh();
		return;
	}
	//Quelle Action, quelle case, quelle nouvelle touche
	FMapPlayerKeyArgs Args;
	Args.MappingName = MappingName;
	Args.Slot = KeySlot;
	Args.NewKey = NewKey;
	
	// Si Enhanced Input refuse, il explique pourquoi dans FailureReason
	FGameplayTagContainer FailureReason;
	UserSettings->MapPlayerKey(Args, FailureReason);
	
	// Les touches sont sauvegardées tout de suite, dans un fichier à part (Saved/SaveGames)
	UserSettings->SaveSettings();
	Refresh();
}
