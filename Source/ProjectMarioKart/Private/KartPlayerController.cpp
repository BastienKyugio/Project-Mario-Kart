#include "KartPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "KartCountdownWidget.h"
#include "KartPawn.h"
#include "KartRaceHudWidget.h"
#include "Item/KartItemWidget.h"


void AKartPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // L'UI n'existe que pour le joueur qui joue sur cette machine
    if (!IsLocalController())
    {
        return;
    }
    
    // On donne au joueur les touches du kart (avant, c'était le kart qui le faisait dans NotifyControllerChanged)
    // Le Mapping Context dit quelle touche déclenche quelle Input Action
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (KartMappingContext != nullptr)
        {
            Subsystem->AddMappingContext(KartMappingContext, 0);
        }
    }
    
    if (CountdownWidgetClass != nullptr)
    {
        CountdownWidget = CreateWidget<UKartCountdownWidget>(this, CountdownWidgetClass);
        if (CountdownWidget != nullptr)
        {
            CountdownWidget->AddToViewport();
        }
    }
    
    // Même recette que les deux autres widgets : créer, vérifier, afficher
    if (ItemWidgetClass != nullptr)
    {
        ItemWidget = CreateWidget<UKartItemWidget>(this, ItemWidgetClass);
        if (ItemWidget != nullptr)
        {
            ItemWidget->AddToViewport();
        }
    }
    
    if (RaceHudWidgetClass != nullptr)
    {
        RaceHudWidget = CreateWidget<UKartRaceHudWidget>(this, RaceHudWidgetClass);
        if (RaceHudWidget != nullptr)
        {
            RaceHudWidget->AddToViewport();
        }
    }
}

void AKartPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // InputComponent appartient au controller : les touches restent branchées même si on change de kart
    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (EnhancedInput == nullptr)
    {
        return;
    }
    
    // Triggered = tant qu'on appuie, Completed = quand on lâche
    EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AKartPlayerController::OnThrottle);
    EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AKartPlayerController::OnThrottleReleased);
    EnhancedInput->BindAction(SteerAction, ETriggerEvent::Triggered, this, &AKartPlayerController::OnSteer);
    EnhancedInput->BindAction(SteerAction, ETriggerEvent::Completed, this, &AKartPlayerController::OnSteerReleased);
    
    // Started = une seule fois au moment où on appuie
    EnhancedInput->BindAction(DriftAction, ETriggerEvent::Started, this, &AKartPlayerController::OnDriftPressed);
    EnhancedInput->BindAction(DriftAction, ETriggerEvent::Completed, this, &AKartPlayerController::OnDriftReleased);
    EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AKartPlayerController::OnJumpPressed);
    EnhancedInput->BindAction(UseItemAction, ETriggerEvent::Started, this, &AKartPlayerController::OnUseItemPressed);
}

AKartPawn* AKartPlayerController::GetKart() const
{
    // GetPawn<AKartPawn>() = le pawn possédé, déjà converti en kart (nullptr si ce n'est pas un kart, ou s'il n'y en a pas encore)
    return GetPawn<AKartPawn>();
}


// ==================== Touches -> commandes ====================
// Chaque fonction lit la touche et donne l'ordre au kart. Une IA appellera les mêmes commandes, sans touches
void AKartPlayerController::OnThrottle(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->SetThrottleInput(Value.Get<float>());
    }
}

void AKartPlayerController::OnThrottleReleased(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->SetThrottleInput(0.f);
    }
}

void AKartPlayerController::OnSteer(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->SetSteerInput(Value.Get<float>());
    }
}

void AKartPlayerController::OnSteerReleased(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->SetSteerInput(0.f);
    }
}

void AKartPlayerController::OnDriftPressed(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->StartDrift();
    }
}

void AKartPlayerController::OnDriftReleased(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->EndDrift();
    }
}

void AKartPlayerController::OnJumpPressed(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->Jump();
    }
}

void AKartPlayerController::OnUseItemPressed(const FInputActionValue& Value)
{
    if (AKartPawn* Kart = GetKart())
    {
        Kart->UseItem();
    }
}
