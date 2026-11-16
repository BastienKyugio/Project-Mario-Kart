#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KartMenuPlayerController.generated.h"

class UKartMainMenuWidget;

UCLASS()
class PROJECTMARIOKART_API AKartMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category ="Kart|UI")
	TSubclassOf<UKartMainMenuWidget> MainMenuWidgetClass;
	
private:
	UPROPERTY()
	TObjectPtr<UKartMainMenuWidget> MainMenuWidget;
	
};