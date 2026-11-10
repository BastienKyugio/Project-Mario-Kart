#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KartPlayerController.generated.h"

class UKartCountdownWidget;
class UKartRaceHudWidget;
class UKartItemWidget;
class UInputMappingContext;
class UInputAction;
class AKartPawn;
struct FInputActionValue;

UCLASS()
class PROJECTMARIOKART_API AKartPlayerController : public APlayerController
{
    GENERATED_BODY()
protected:
    virtual void BeginPlay() override;
    
    virtual void SetupInputComponent() override;
    
    UPROPERTY(EditAnywhere, Category = "Kart|UI")
    TSubclassOf<UKartCountdownWidget> CountdownWidgetClass;
    
    UPROPERTY(EditAnywhere, Category = "Kart|UI")
    TSubclassOf<UKartItemWidget> ItemWidgetClass;
    
    UPROPERTY(EditAnywhere, Category = "Kart|UI")
    TSubclassOf<UKartRaceHudWidget> RaceHudWidgetClass;
    
    UPROPERTY(EditAnywhere, Category = "Kart|Input")
    TObjectPtr<UInputMappingContext> KartMappingContext;
    
    UPROPERTY(EditAnywhere, Category = "Kart|Input")
    TObjectPtr<UInputAction> ThrottleAction;
    
    UPROPERTY(EditAnywhere, Category = "Kart|Input")
    TObjectPtr<UInputAction> SteerAction;
    
    UPROPERTY(EditAnywhere, Category = "Kart|Input")
    TObjectPtr<UInputAction> DriftAction;
    
    UPROPERTY(EditAnywhere, Category = "Kart|Input")
    TObjectPtr<UInputAction> JumpAction;
    
    UPROPERTY(EditAnywhere, Category = "Kart|Input")
    TObjectPtr<UInputAction> UseItemAction;

private:
    void OnThrottle(const FInputActionValue& Value);
    void OnThrottleReleased(const FInputActionValue& Value);
    void OnSteer(const FInputActionValue& Value);
    void OnSteerReleased(const FInputActionValue& Value);
    void OnDriftPressed(const FInputActionValue& Value);
    void OnDriftReleased(const FInputActionValue& Value);
    void OnJumpPressed(const FInputActionValue& Value);
    void OnUseItemPressed(const FInputActionValue& Value);
    
    AKartPawn* GetKart() const;
    
    UPROPERTY()
    TObjectPtr<UKartCountdownWidget> CountdownWidget;
    
    UPROPERTY()
    TObjectPtr<UKartItemWidget> ItemWidget;
    
    UPROPERTY()
    TObjectPtr<UKartRaceHudWidget> RaceHudWidget;
};
