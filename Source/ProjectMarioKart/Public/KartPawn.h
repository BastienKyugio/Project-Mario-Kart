#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KartPawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UKartItemComponent;

UCLASS()
class PROJECTMARIOKART_API AKartPawn : public APawn
{
    GENERATED_BODY()
public:
    AKartPawn();
    virtual void Tick(float DeltaTime) override;
    // Bloque ou débloque le kart (utilisé par le GameMode pour le compte à rebours)
    UFUNCTION(BlueprintCallable, Category = "Kart")
    void SetCanMove(bool bInCanMove);
    UFUNCTION(BlueprintPure, Category = "Kart")
    bool CanMove() const { return bCanMove; }
    // Direction dans laquelle le kart se déplace vraiment :
    // vers l'avant du kart quand il avance, vers l'arrière en marche arrière, (0,0,0) à l'arrêt
    // (utilisée par les ALapZone pour savoir si on les traverse dans le bon sens)
    UFUNCTION(BlueprintPure, Category = "Kart")
    FVector GetMoveDirection() const;
    // ---------- Commandes (envoyées par le controller : le joueur, ou plus tard une IA) ----------
    // Le kart ne lit aucune touche lui-même : il obéit à ces fonctions, peu importe qui les appelle
    // Accélérateur : +1 = accélérer, -1 = freiner / reculer, 0 = rien
    UFUNCTION(BlueprintCallable, Category = "Kart|Commands")
    void SetThrottleInput(float Value);
    // Volant : +1 = droite, -1 = gauche, 0 = tout droit
    UFUNCTION(BlueprintCallable, Category = "Kart|Commands")
    void SetSteerInput(float Value);
    // Commence un drift (il faut être au sol, assez rapide, et tourner d'un côté)
    UFUNCTION(BlueprintCallable, Category = "Kart|Commands")
    void StartDrift();
    // Termine le drift : donne un turbo si le drift a été assez long
    UFUNCTION(BlueprintCallable, Category = "Kart|Commands")
    void EndDrift();
    // Petit saut (seulement depuis le sol)
    UFUNCTION(BlueprintCallable, Category = "Kart|Commands")
    void Jump();
    // Utilise l'objet bonus en main
    UFUNCTION(BlueprintCallable, Category = "Kart|Commands")
    void UseItem();
    // ---------- Course : tours ----------
    // Appelée par une ALapZone quand le kart la traverse dans le bon sens
    // ZoneIndex 0 = ligne d'arrivée, 1, 2, 3... = checkpoints
    void EnterLapZone(int32 ZoneIndex);
    // Termine le tour en cours : passe au tour suivant, ou finit la course si c'était le dernier tour
    UFUNCTION(BlueprintCallable, Category = "Kart|Race")
    void CompleteLap();
    // Remet le kart au tour 1 (appelée par le GameMode quand une course recommence)
    UFUNCTION(BlueprintCallable, Category = "Kart|Race")
    void ResetRaceProgress();
    UFUNCTION(BlueprintPure, Category = "Kart|Race")
    int32 GetCurrentLap() const { return CurrentLap; }
    UFUNCTION(BlueprintPure, Category = "Kart|Race")
    bool HasFinishedRace() const { return bHasFinishedRace; }
    // Temps de course au moment où le kart a passé la ligne du dernier tour
    UFUNCTION(BlueprintPure, Category = "Kart|Race")
    float GetFinishTime() const { return FinishTime; }
    // ---------- Objets bonus ----------
    // Lance un turbo de Duration secondes : le même que celui du drift
    // StartBoost reste privée, ApplyBoost est la "porte d'entrée" pour les autres classes (les objets bonus)
    UFUNCTION(BlueprintCallable, Category = "Kart|Items")
    void ApplyBoost(float Duration);
    
    // Le kart est touché (piège, projectile) : il part en tête-à-queue pendant Duration secondes
    UFUNCTION(BlueprintCallable, Category = "Kart|Items")
    void SpinOut(float Duration);
    
    UFUNCTION(BlueprintPure, Category = "Kart|Items")
    bool IsSpinningOut() const { return SpinOutTimeRemaining > 0.f; }
    
    UFUNCTION(BlueprintPure, Category = "Kart|Items")
    UKartItemComponent* GetItemComponent() const { return ItemComponent; }
protected:
    virtual void BeginPlay() override;
    // ---------- Composants ----------
    // Sphère de collision : c'est elle qui se déplace et qui bloque contre les murs
    // (pas de coins, donc rien n'accroche dans les pentes et les bosses)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<USphereComponent> CollisionSphere;
    // Châssis du kart
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UStaticMeshComponent> BodyMesh;
    // Les 4 roues (les roues avant braquent)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UStaticMeshComponent> WheelFrontLeft;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UStaticMeshComponent> WheelFrontRight;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UStaticMeshComponent> WheelRearLeft;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UStaticMeshComponent> WheelRearRight;
    // Bras de caméra (avec du retard pour un effet de poursuite)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<USpringArmComponent> SpringArm;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UCameraComponent> Camera;
    // La "poche" à objets bonus : c'est elle qui garde l'objet en main et l'utilise
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
    TObjectPtr<UKartItemComponent> ItemComponent;
    // ---------- Réglages : déplacement (cm et secondes) ----------
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float MaxForwardSpeed = 2500.f;
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float MaxReverseSpeed = 800.f;
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float Acceleration = 1500.f;
    // Décélération quand on appuie dans le sens opposé à la vitesse
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float BrakeDeceleration = 3000.f;
    // Décélération quand on lâche les gaz
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float CoastDeceleration = 800.f;
    // Vitesse de rotation en degrés par seconde
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float TurnRate = 100.f;
    // En dessous de cette vitesse, le kart ne tourne pas
    UPROPERTY(EditAnywhere, Category = "Kart|Movement")
    float MinSpeedToTurn = 50.f;
    // Part de la vitesse conservée quand on touche un mur (0 = arrêt net)
    UPROPERTY(EditAnywhere, Category = "Kart|Movement", meta = (ClampMin = "0", ClampMax = "1"))
    float WallSpeedFactor = 0.3f;
    // ---------- Réglages : gravité et sol ----------
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float GravityStrength = 2500.f;
    // Distance sur laquelle on cherche le sol sous les roues
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float GroundTraceDistance = 80.f;
    // Hauteur (en cm) du centre de la sphère au-dessus du sol
    // Plus grande que le rayon de la sphère : elle flotte au-dessus des petites bosses, ce sont les roues qui suivent le sol
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float RideHeight = 45.f;
    // Si on était au sol, on y reste collé tant que le sol est à moins de cette distance (évite de décoller en haut des bosses)
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float GroundStickDistance = 30.f;
    // Vitesse à laquelle le kart s'incline pour suivre la pente
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float GroundAlignSpeed = 8.f;
    // Distance (en cm) entre le centre et les roues avant/arrière, pour les 4 rayons qui lisent la pente
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float GroundProbeHalfLength = 50.f;
    // Distance (en cm) entre le centre et les roues gauche/droite
    UPROPERTY(EditAnywhere, Category = "Kart|Gravity")
    float GroundProbeHalfWidth = 50.f;
    // ---------- Réglages : saut ----------
    // Vitesse vers le haut au moment du saut (450 = environ 40 cm de haut)
    UPROPERTY(EditAnywhere, Category = "Kart|Jump")
    float JumpVelocity = 450.f;
    // ---------- Réglages : drift et boost ----------
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float DriftMinSpeed = 1000.f;
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float DriftTurnMultiplier = 1.4f;
    // Angle visuel du châssis pendant le drift (en degrés)
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float DriftSlideAngle = 25.f;
    // Temps (en secondes) pour entrer complètement dans le drift : virage et angle du châssis montent progressivement
    UPROPERTY(EditAnywhere, Category = "Kart|Drift", meta = (ClampMin = "0.01"))
    float DriftEnterTime = 0.4f;
    // Vitesse à laquelle le châssis rejoint son angle (plus petit = plus doux)
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float DriftSlideInterpSpeed = 6.f;
    // Temps de drift pour obtenir le mini-turbo (bleu)
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float MiniTurboChargeTime = 1.f;
    // Temps de drift pour obtenir le super-turbo (orange)
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float SuperTurboChargeTime = 2.f;
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float MiniTurboDuration = 0.6f;
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float SuperTurboDuration = 1.2f;
    UPROPERTY(EditAnywhere, Category = "Kart|Drift")
    float BoostSpeed = 3500.f;
    // ---------- Réglages : visuel ----------
    UPROPERTY(EditAnywhere, Category = "Kart|Visual")
    float WheelRadius = 20.f;
    UPROPERTY(EditAnywhere, Category = "Kart|Visual")
    float MaxWheelSteerAngle = 30.f;
    // Champ de vision (FOV) à l'arrêt
    UPROPERTY(EditAnywhere, Category = "Kart|Camera")
    float BaseFieldOfView = 90.f;
    // FOV à vitesse max : plus large = sensation de vitesse
    UPROPERTY(EditAnywhere, Category = "Kart|Camera")
    float MaxSpeedFieldOfView = 100.f;
    // FOV pendant un boost
    UPROPERTY(EditAnywhere, Category = "Kart|Camera")
    float BoostFieldOfView = 110.f;
    // Part de l'angle de drift que la caméra suit (1 = toujours derrière l'arrière du kart, 0 = ne suit pas)
    UPROPERTY(EditAnywhere, Category = "Kart|Camera", meta = (ClampMin = "0", ClampMax = "1"))
    float CameraDriftFollow = 1.f;
    // Vitesse à laquelle la caméra rattrape l'angle du châssis (plus petit = la caméra traîne plus)
    UPROPERTY(EditAnywhere, Category = "Kart|Camera")
    float CameraDriftInterpSpeed = 3.f;
    // En dessous de cette pente (en degrés), la caméra ne s'incline pas du tout
    UPROPERTY(EditAnywhere, Category = "Kart|Camera", meta = (ClampMin = "0", ClampMax = "90"))
    float CameraSlopeDeadZone = 10.f;
    // Vitesse à laquelle la caméra s'incline dans les grandes pentes
    UPROPERTY(EditAnywhere, Category = "Kart|Camera")
    float CameraSlopeFollowSpeed = 3.f;
    // Vitesse à laquelle le FOV change
    UPROPERTY(EditAnywhere, Category = "Kart|Camera")
    float FieldOfViewInterpSpeed = 4.f;
    

    UPROPERTY(EditAnywhere, Category = "Kart|SpinOut", meta = (ClampMin = "0"))
    int32 SpinOutTurns = 2;
    
    UPROPERTY(EditAnywhere, Category = "Kart|SpinOut", meta = (ClampMin = "0", ClampMax = "1"))
    float SpinOutSpeedFactor = 0.6f;


    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float CurrentSpeed = 0.f;
    
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float VerticalSpeed = 0.f;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float ThrottleInput = 0.f;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float SteerInput = 0.f;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    bool bIsGrounded = false;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    bool bIsDrifting = false;
    
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    int32 DriftDirection = 0;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float DriftChargeTime = 0.f;
    // 0 = début du drift, 1 = drift complètement engagé (monte en DriftEnterTime secondes)
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float DriftBlend = 0.f;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float BoostTimeRemaining = 0.f;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float WheelSpinAngle = 0.f;
    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    FVector GroundNormal = FVector::UpVector;
    // Faux pendant le compte à rebours : le kart ne répond pas aux touches (c'est le GameMode qui le change)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
    bool bCanMove = true;

    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float SpinOutTimeRemaining = 0.f;

    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float SpinOutTotalTime = 0.f;

    UPROPERTY(VisibleAnywhere, Category = "Kart|State")
    float SpinOutYaw = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
    int32 CurrentLap = 1;
    // Numéro du prochain checkpoint à traverser (repart à 1 à chaque tour)
    // Quand il dépasse le nombre de checkpoints, tous ont été passés : la ligne d'arrivée peut valider le tour
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
    int32 NextCheckpoint = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
    bool bHasFinishedRace = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
    float FinishTime = 0.f;
private:
    // ---------- Étapes du Tick (dans l'ordre) ----------
    void UpdateGround(float DeltaTime);
    void UpdateSpeed(float DeltaTime);
    void UpdateSteering(float DeltaTime);
    void UpdateDrift(float DeltaTime);
    void UpdateBoost(float DeltaTime);
    void MoveKart(float DeltaTime);
    void AlignToGround(float DeltaTime);
    void UpdateVisuals(float DeltaTime);
    void UpdateCameraFieldOfView(float DeltaTime);
    void ShowDebugInfo() const;
    // Lance un rayon sous chaque roue : renvoie le nombre de roues qui touchent le sol,
    // le point moyen du sol et sa pente (plus stable qu'un seul point de contact)
    int32 TraceGroundUnderWheels(FVector& OutGroundPoint, FVector& OutGroundNormal) const;
    // ---------- Boost ----------
    void StartBoost(float Duration);
    float GetCurrentMaxSpeed() const;
    // ---------- Visuel des roues ----------
    void SetWheelRotation(UStaticMeshComponent* Wheel, const FRotator& BaseRotation, 
        float SteerAngle) const;
    // ---------- Tête-à-queue ----------

    void UpdateSpinOut(float DeltaTime);

    bool bShowDebug = false;

    FRotator BodyBaseRotation;
    FRotator SpringArmBaseRotation;
    FRotator WheelFrontLeftBaseRotation;
    FRotator WheelFrontRightBaseRotation;
    FRotator WheelRearLeftBaseRotation;
    FRotator WheelRearRightBaseRotation;

    
    float BodySlideYaw = 0.f;

    float CameraSlideYaw = 0.f;

    float CameraSlopePitch = 0.f;

    int32 SphereFloorHitCount = 0;
};