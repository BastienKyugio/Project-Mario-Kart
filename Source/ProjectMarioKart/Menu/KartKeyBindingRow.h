#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "KartKeyBindingRow.generated.h"

class UTextBlock;
class UInputKeySelector;

UCLASS()
class PROJECTMARIOKART_API UKartKeyBindingRow : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Le nom de la touche réglable
	void SetMappingName(FName InMappingName);
	
	//Relit les touches actuelles dans les réglages des input et les affiche
	void Refresh();
	
protected:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ActionName;
	
	//La touche du clavier
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> KeyboardKeySelector;
	
	//Le bouton de la manette
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> GamepadKeySelector;
	
private:
	UFUNCTION()
	void HandleKeyboardKeySelected(FInputChord SelectedKey);
	
	UFUNCTION()
	void HandleGamepadKeySelected(FInputChord SelectedKey);
	
	//Demande à Enhanced Input de mettre NewKey dans la case KeySlot, puis sauvegarde
	void MapKey(EPlayerMappableKeySlot KeySlot, const FKey& NewKey);
	
	//Les réglages de touches du joueur qui possède ce widget
	UEnhancedInputUserSettings* GetUserSettings() const;
	
	FName MappingName;
	
};