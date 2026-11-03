#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KartPlayerController.generated.h"

class UKartCountdownWidget;
class UKartRaceHudWidget;

// Controller du joueur : crée l'UI de la course (seulement pour le joueur local)
UCLASS()
class PROJECTMARIOKART_API AKartPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Widget du compte à rebours à afficher (mettre WBP_Countdown ici dans BP_KartPlayerController)
	UPROPERTY(EditAnywhere, Category = "Kart|UI")
	TSubclassOf<UKartCountdownWidget> CountdownWidgetClass;

	// Widget du chrono et des tours (mettre WBP_RaceHud ici dans BP_KartPlayerController)
	UPROPERTY(EditAnywhere, Category = "Kart|UI")
	TSubclassOf<UKartRaceHudWidget> RaceHudWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UKartCountdownWidget> CountdownWidget;

	UPROPERTY()
	TObjectPtr<UKartRaceHudWidget> RaceHudWidget;
};
 