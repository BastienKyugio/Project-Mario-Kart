#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KartPauseMenuWidget.generated.h"

class UButton;
class UKartOptionWidget;

UCLASS()
class PROJECTMARIOKART_API UKartPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void FocusDefaultButton();
	
protected:
	virtual void NativeOnInitialized() override;
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ResumeButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> OptionsButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RestartButton;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> MainMenuButton;
	
	//Le meme ecran option que le menu principal
	UPROPERTY(EditAnywhere, Category= "Kart|Menu")
	TSubclassOf<class UKartOptionWidget> OptionsWidgetClass;
	
	UPROPERTY(EditAnywhere, Category= "Kart|Menu")
	TSoftObjectPtr<UWorld> MainMenuLevel;
	
private:
	UFUNCTION()
	void HandleResumeClicked(); 
	
	UFUNCTION()
	void HandleOptionsClicked();
	
	UFUNCTION()
	void HandleRestartClicked();
	
	UFUNCTION()
	void HandleMainMenuClicked();
	
	UFUNCTION()
	void HandleOptionsClosed();
	
	UPROPERTY()
	TObjectPtr<UKartOptionWidget> OptionsWidget;
	
};