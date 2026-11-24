#include "Menu/KartOptionWidget.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Menu/KartKeyBindingRow.h"
#include "Menu/KartOptionSelector.h"
#include "Menu/KartOptionSlider.h"
#include "Settings/KartGameUserSettings.h"

void UKartOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// Les onglets
	GraphicsTabButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleGraphicsTabClicked);
	AudioTabButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleAudioTabClicked);
	GameTabButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleGameTabClicked);
	ControlsTabButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleAudioTabClicked);
	
	//graphisme
	SetupGraphicsChoices();
	ResolutionSelector->OnSelectionChanged.AddDynamic(this, &UKartOptionWidget::HandleResolutionChanged);
	WindowModeSelector->OnSelectionChanged.AddDynamic(this, &UKartOptionWidget::HandleWindowModeChanged);
	QualitySelector->OnSelectionChanged.AddDynamic(this, &UKartOptionWidget::HandleQualityChanged);
	VSyncSelector->OnSelectionChanged.AddDynamic(this, &UKartOptionWidget::HandleVSyncChanged);
	FrameRateSelector->OnSelectionChanged.AddDynamic(this, &UKartOptionWidget::HandleFrameRateChanged);
	
	//Audio
	MasterVolumeSlider->SetRange(0.f, 1.f, 0.05f);
	MusicVolumeSlider->SetRange(0.0f, 1.f, 0.05f);
	SfxVolumeSlider->SetRange(0.0f, 1.f, 0.05f);
	MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UKartOptionWidget::HandleMasterVolumeChanged);
	MusicVolumeSlider->OnValueChanged.AddDynamic(this, &UKartOptionWidget::HandleMusicVolumeChanged);
	SfxVolumeSlider->OnValueChanged.AddDynamic(this, &UKartOptionWidget::HandleSfxVolumeChanged);
	
	//Jeu
	FieldOfViewSlider->SetRange(UKartGameUserSettings::MinFieldOfView, UKartGameUserSettings::MaxFieldOfView, 1.f);
	LapCountSlider->SetRange(UKartGameUserSettings::MinLapCount, UKartGameUserSettings::MaxLapCount, 1.f);
	FieldOfViewSlider->OnValueChanged.AddDynamic(this, &UKartOptionWidget::HandleFieldOfViewChanged);
	LapCountSlider->OnValueChanged.AddDynamic(this, &UKartOptionWidget::HandleLapCountChanged);
	
	DifficultySelector->SetChoices({FText::FromString(TEXT("Facile")), FText::FromString(TEXT("Normale")), FText::FromString(TEXT("Difficile")) });
	DifficultySelector->OnSelectionChanged.AddDynamic(this, &UKartOptionWidget::HandleDifficultyChanged);
	
	BuildKeyBindingRow();
	
	//Les boutons du bas et de confirmation
	ApplyButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleApplyClicked);
	DefaultButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleDefaultClicked);
	BackButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleBackClicked);
	KeepButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleKeepClicked);
	RevertButton->OnClicked.AddDynamic(this, &UKartOptionWidget::HandleRevertClicked);
	
	ShowConfirmPannel(false);
	ShowTab(0);
	LoadFromSettings();
}

void UKartOptionWidget::FocusDefaultButton()
{
	GraphicsTabButton->SetUserFocus(GetOwningPlayer());
}


void UKartOptionWidget::HandleGraphicsTabClicked()
{
	ShowTab(0);
}

void UKartOptionWidget::HandleAudioTabClicked()
{
	ShowTab(1);
}

void UKartOptionWidget::HandleGameTabClicked()
{
	ShowTab(2);
}

void UKartOptionWidget::HandleControlsTabClicked()
{
	ShowTab(3);
}

void UKartOptionWidget::ShowTab(int32 TabIndex)
{
	TabSwitcher->SetActiveWidgetIndex(TabIndex);
}

FReply UKartOptionWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	
	if (Key==EKeys::Escape || Key==EKeys::Gamepad_FaceButton_Right)
	{
		if (ConfirmTimeLeft > 0.f)
		{
			HandleRevertClicked();
		}
		else
		{
			HandleBackClicked();
		}
		return FReply::Handled();
	}
	
	if (Key == EKeys::Gamepad_LeftShoulder || Key == EKeys::Gamepad_RightShoulder)
	{
		const int32 TabCount = TabSwitcher->GetNumWidgets();
		const int32 Direction = Key == EKeys::Gamepad_LeftShoulder ? -1 : 1;
		ShowTab((TabSwitcher->GetActiveWidgetIndex() + Direction + TabCount) % TabCount);
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry,InKeyEvent);
}

void UKartOptionWidget::SetupGraphicsChoices()
{
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	
	if (const UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Resolutions.AddUnique(Settings->GetScreenResolution());
	}
	TArray<FText> ResolutionsChoices;
	for (const FIntPoint& Resolution : Resolutions)
	{
		ResolutionsChoices.Add(FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y)));
	}
	ResolutionSelector->SetChoices(ResolutionsChoices);
	
	WindowModeSelector->SetChoices({FText::FromString(TEXT("Plein écran")), FText::FromString(TEXT("Plein écran fenêtré")), FText::FromString(TEXT("Fenêtré")) });
	
	QualitySelector->SetChoices({FText::FromString(TEXT("Basse")), FText::FromString(TEXT("Moyenne")), FText::FromString(TEXT("Haute")), FText::FromString(TEXT("Épique")) });
	
	VSyncSelector->SetChoices({FText::FromString(TEXT("Non")), FText::FromString(TEXT("Oui")) });
	
	TArray<FText> FrameRateChoices;
	for (const float Limit : FrameRatesLimits)
	{
		FrameRateChoices.Add(Limit > 0.f ? FText::AsNumber(FMath::RoundToInt(Limit)) : FText::FromString(TEXT("Illimité")));
	}
	FrameRateSelector->SetChoices(FrameRateChoices);
}

void UKartOptionWidget::LoadFromSettings()
{
	const UKartGameUserSettings* Settings = UKartGameUserSettings::Get();
	if (Settings ==nullptr)
	{
		return;
	}
	
	//Graphismes
	ResolutionSelector->SetSelectedIndex(Resolutions.Find(Settings->GetScreenResolution()));
	WindowModeSelector->SetSelectedIndex(static_cast<int32>(Settings->GetFullscreenMode()));
	//-1 = Des reglages perso
	const int32 Quality = Settings->GetOverallScalabilityLevel();
	QualitySelector->SetSelectedIndex(Quality >= 0 ? Quality : 3);
	VSyncSelector->SetSelectedIndex(FrameRatesLimits.Find(Settings->GetFrameRateLimit()));
	FrameRateSelector->SetSelectedIndex(FrameRatesLimits.Find(Settings->GetFrameRateLimit()));
	
	//Audio
	MasterVolumeSlider->SetValue(Settings->GetMasterVolume());
	MusicVolumeSlider->SetValue(Settings->GetMusicVolume());
	SfxVolumeSlider->SetValue(Settings->GetSfxVolume());
	
	//Jeu
	FieldOfViewSlider->SetValue(Settings->GetCameraFieldOfView());
	LapCountSlider->SetValue(Settings->GetLapCount());
	DifficultySelector->SetSelectedIndex(Settings->GetDifficulty());
}

void UKartOptionWidget::BuildKeyBindingRow()
{
	if (KeyBindingRowClass == nullptr || KeyBindingContext == nullptr)
	{
		return;
	}
	const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer != nullptr ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
	UEnhancedInputUserSettings* UserSettings = Subsystem != nullptr ? Subsystem->GetUserSettings() : nullptr;
	if (UserSettings == nullptr)
	{
		return;
	}
	UserSettings->RegisterInputMappingContext(KeyBindingContext);
	
	KeyBindingList->ClearChildren();
	for (const FName& MappingName : KeyMappingsNames)
	{
		UKartKeyBindingRow* Row = CreateWidget<UKartKeyBindingRow>(this, KeyBindingRowClass);
		KeyBindingList->AddChild(Row);
		Row->SetMappingName(MappingName);
	}
	
}

void UKartOptionWidget::HandleResolutionChanged(int32 NewIndex)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get(); Settings != nullptr && Resolutions.IsValidIndex(NewIndex))
	{
		Settings->SetScreenResolution(Resolutions[NewIndex]);
	}
}


void UKartOptionWidget::HandleWindowModeChanged(int32 NewIndex)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetFullscreenMode(static_cast<EWindowMode::Type>(NewIndex));
	}
}

void UKartOptionWidget::HandleQualityChanged(int32 NewIndex)
{
	if (UKartGameUserSettings*Settings = UKartGameUserSettings::Get())
	{
		Settings->SetOverallScalabilityLevel(NewIndex);
	}
}

void UKartOptionWidget::HandleVSyncChanged(int32 NewIndex)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetVSyncEnabled(NewIndex == 1);
	}
}

void UKartOptionWidget::HandleFrameRateChanged(int32 NewIndex)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get(); Settings != nullptr && FrameRatesLimits.IsValidIndex(NewIndex))
	{
		Settings->SetFrameRateLimit(FrameRatesLimits[NewIndex]);
	}
}

void UKartOptionWidget::HandleMasterVolumeChanged(float NewValue)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetMasterVolume(NewValue);
		Settings->ApplyAudioSettings(this);
	}
}

void UKartOptionWidget::HandleMusicVolumeChanged(float NewValue)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetMusicVolume(NewValue);
		Settings->ApplyAudioSettings(this);
	}
}

void UKartOptionWidget::HandleSfxVolumeChanged(float NewValue)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetSfxVolume(NewValue);
		Settings->ApplyAudioSettings(this);
	}
}

// Jeu

void UKartOptionWidget::HandleFieldOfViewChanged(float NewValue)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetCameraFieldOfView(NewValue);
	}
}

void UKartOptionWidget::HandleLapCountChanged(float NewValue)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetLapCount(FMath::RoundToInt(NewValue));
	}
}

void UKartOptionWidget::HandleDifficultyChanged(int32 NewIndex)
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->SetDifficulty(NewIndex);
	}
}

// Boutons du bas
void UKartOptionWidget::HandleApplyClicked()
{
	UKartGameUserSettings* Settings = UKartGameUserSettings::Get();
	if (Settings == nullptr)
	{
		return;
	}
	
	const bool bVideoModeChanged = Settings->IsScreenResolutionDirty() || Settings->IsFullscreenModeDirty();
	
	Settings->ApplySettings(false);
	
	if (bVideoModeChanged)
	{
		ShowConfirmPannel(true);
	}
}

void UKartOptionWidget::HandleDefaultClicked()
{
	UKartGameUserSettings* Settings = UKartGameUserSettings::Get();
	if (Settings == nullptr)
	{
		return;
	}
	
	Settings->SetToDefaults();
	Settings->ApplyAudioSettings(this);
	LoadFromSettings();
	
	const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	const UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer != nullptr ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
	if (UEnhancedInputUserSettings* UserSettings = Subsystem != nullptr ? Subsystem->GetUserSettings() : nullptr)
	{
		UserSettings->GetActiveKeyProfile()->ResetToDefault();
		UserSettings->ApplySettings();
		UserSettings->SaveSettings();
	}
	BuildKeyBindingRow();
}



void UKartOptionWidget::HandleBackClicked()
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->LoadSettings(true);
		Settings->ApplyAudioSettings(this);
	}
	
	RemoveFromParent();
	OnClosed.Broadcast();
}

void UKartOptionWidget::ShowConfirmPannel(bool bShow)
{
	ConfirmPannel->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ConfirmTimeLeft = bShow ? ConfirmDuration : 0.f;
	
	if (bShow)
	{
		KeepButton->SetUserFocus(GetOwningPlayer());
	}
	else
	{
		FocusDefaultButton();
	}
}

void UKartOptionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (ConfirmTimeLeft <= 0.f)
	{
		HandleRevertClicked();
	}
}

void UKartOptionWidget::HandleKeepClicked()
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->ConfirmVideoMode();
		Settings->SaveSettings();
	}
	ShowConfirmPannel(false);
}

void UKartOptionWidget::HandleRevertClicked()
{
	if (UKartGameUserSettings* Settings = UKartGameUserSettings::Get())
	{
		Settings->RevertVideoMode();
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
	ShowConfirmPannel(false);
	LoadFromSettings();
}
