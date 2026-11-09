#include "KartPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "KartGameMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/KartItemComponent.h"

// Marge (en cm) sous les roues pour considérer que le kart touche le sol
static constexpr float GroundSnapTolerance = 5.f;
// Une surface dont la normale a un Z plus grand que ça est un sol, sinon c'est un mur
static constexpr float WalkableFloorZ = 0.7f;

AKartPawn::AKartPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    // La sphère de collision est la racine : c'est elle q    u'on déplace
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetSphereRadius(35.f);
    CollisionSphere->SetCollisionProfileName(TEXT("Pawn"));
    CollisionSphere->SetSimulatePhysics(false);
    RootComponent = CollisionSphere;
    // Les meshes sont juste visuels : pas de collision, c'est la sphère qui s'en occupe
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(CollisionSphere);
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // Les roues sont attachées au châssis pour suivre son décalage pendant le drift
    WheelFrontLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelFrontLeft"));
    WheelFrontLeft->SetupAttachment(BodyMesh);
    WheelFrontLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WheelFrontRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelFrontRight"));
    WheelFrontRight->SetupAttachment(BodyMesh);
    WheelFrontRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WheelRearLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelRearLeft"));
    WheelRearLeft->SetupAttachment(BodyMesh);
    WheelRearLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WheelRearRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelRearRight"));
    WheelRearRight->SetupAttachment(BodyMesh);
    WheelRearRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // La poche à objets : un ActorComponent n'a pas de position, donc pas de SetupAttachment
    ItemComponent = CreateDefaultSubobject<UKartItemComponent>(TEXT("ItemComponent"));
    // Caméra derrière et au-dessus du kart
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(CollisionSphere);
    SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
    SpringArm->TargetArmLength = 600.f;
    SpringArm->SocketOffset = FVector(0.f, 0.f, 100.f);
    // Pas de retard en position : à grande vitesse il fait trembler le kart d'avant en arrière à l'écran
    SpringArm->bEnableCameraLag = false;
    // Retard en rotation seulement : la caméra suit les virages en douceur
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 6.f;
    SpringArm->bInheritRoll = false;
    // La caméra ne copie pas l'inclinaison du kart : on choisit nous-mêmes son angle (voir UpdateVisuals)
    SpringArm->bInheritPitch = false;
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}
void AKartPawn::BeginPlay()
{
    Super::BeginPlay();
    // On retient les rotations placées dans le Blueprint pour ne pas les écraser
    BodyBaseRotation = BodyMesh->GetRelativeRotation();
    SpringArmBaseRotation = SpringArm->GetRelativeRotation();
    WheelFrontLeftBaseRotation = WheelFrontLeft->GetRelativeRotation();
    WheelFrontRightBaseRotation = WheelFrontRight->GetRelativeRotation();
    WheelRearLeftBaseRotation = WheelRearLeft->GetRelativeRotation();
    WheelRearRightBaseRotation = WheelRearRight->GetRelativeRotation();
    // Si la course n'a pas encore commencé, le kart attend le GO du GameMode
    // (sans KartGameMode, le kart roule tout de suite)
    if (const AKartGameMode* GameMode = GetWorld()->GetAuthGameMode<AKartGameMode>())
    {
        bCanMove = GameMode->IsRaceStarted();
        // On lit une seule fois la case de debug du GameMode, au lieu de la redemander à chaque frame dans Tick
        bShowDebug = GameMode->IsDebugTextEnabled();
    }
}
void AKartPawn::SetCanMove(bool bInCanMove)
{
    bCanMove = bInCanMove;
}
FVector AKartPawn::GetMoveDirection() const
{
    // Sign donne le signe d'un nombre : +1 s'il est positif, -1 s'il est négatif, 0 s'il vaut 0
    // CurrentSpeed est négative en marche arrière : Sign retourne alors la direction vers l'arrière du kart
    // On le choisit plutôt qu'un "if" car il gère les 3 cas (avant, arrière, arrêt) en une seule ligne
    return GetActorForwardVector() * FMath::Sign(CurrentSpeed);
}
// ==================== Course : tours ====================
void AKartPawn::EnterLapZone(int32 ZoneIndex)
{
    // GetAuthGameMode = le GameMode de la partie, converti en AKartGameMode (nullptr si c'est un autre GameMode)
    const AKartGameMode* GameMode = GetWorld()->GetAuthGameMode<AKartGameMode>();
    // Pas de GameMode de kart, course pas encore lancée ou déjà finie : les zones ne comptent pas
    if (GameMode == nullptr || !GameMode->IsRaceStarted() || bHasFinishedRace)
    {
        return;
    }
    if (ZoneIndex == NextCheckpoint)
    {
        // C'est le checkpoint attendu : on attend maintenant le suivant
        ++NextCheckpoint;
    }
    else if (ZoneIndex == 0 && NextCheckpoint > GameMode->GetCheckpointCount())
    {
        // Ligne d'arrivée, et tous les checkpoints du tour ont été passés : le tour est fini
        CompleteLap();
    }
    // Sinon on ignore : checkpoint pas dans l'ordre (raccourci ou demi-tour),
    // ou ligne d'arrivée sans avoir fait le tour (allers-retours sur la ligne)
}
void AKartPawn::CompleteLap()
{
    AKartGameMode* GameMode = GetWorld()->GetAuthGameMode<AKartGameMode>();
    if (GameMode == nullptr || bHasFinishedRace)
    {
        return;
    }
    // Nouveau tour : il faudra repasser tous les checkpoints depuis le premier
    NextCheckpoint = 1;
    // C'était le dernier tour : la course est finie pour ce kart
    if (CurrentLap >= GameMode->GetLapCount())
    {
        bHasFinishedRace = true;
        FinishTime = GameMode->GetRaceTime();
        GameMode->NotifyKartFinished(this);
        return;
    }
    ++CurrentLap;
}
void AKartPawn::ResetRaceProgress()
{
    CurrentLap = 1;
    NextCheckpoint = 1;
    bHasFinishedRace = false;
    FinishTime = 0.f;
}
void AKartPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Kart bloqué (compte à rebours) : on ignore les touches, mais la gravité continue de marcher
    // Les touches sont relues dès la frame suivante, on peut donc garder l'accélérateur enfoncé pendant le décompte
    if (!bCanMove)
    {
        ThrottleInput = 0.f;
        SteerInput = 0.f;
    }
    // Tête-à-queue : il doit passer avant les autres étapes, car il efface les commandes du joueur
    UpdateSpinOut(DeltaTime);
    // Chaque étape est une petite fonction, appelée dans l'ordre
    UpdateGround(DeltaTime);
    UpdateBoost(DeltaTime);
    UpdateSpeed(DeltaTime);
    UpdateSteering(DeltaTime);
    UpdateDrift(DeltaTime);
    MoveKart(DeltaTime);
    AlignToGround(DeltaTime);
    UpdateVisuals(DeltaTime);
    UpdateCameraFieldOfView(DeltaTime);
    // Le debug doit être réécrit à chaque frame (les messages ne durent qu'une frame),
    // mais la case, elle, a été lue une seule fois dans BeginPlay
    if (bShowDebug)
    {
        ShowDebugInfo();
    }
}
// ==================== Commandes ====================
// Appelées par le controller (le joueur, ou plus tard une IA) : le kart ne sait pas qui le pilote
void AKartPawn::SetThrottleInput(float Value)
{
    // Clamp garde la valeur entre -1 et 1 : aucun controller ne peut demander d'accélérer « 5 fois plus fort »
    ThrottleInput = FMath::Clamp(Value, -1.f, 1.f);
}
void AKartPawn::SetSteerInput(float Value)
{
    SteerInput = FMath::Clamp(Value, -1.f, 1.f);
}
void AKartPawn::UseItem()
{
    // Pas d'objet pendant le compte à rebours ni pendant un tête-à-queue
    if (bCanMove && !IsSpinningOut())
    {
        // Le kart ne sait pas utiliser les objets lui-même : il passe la main à son composant
        ItemComponent->UseItem();
    }
}
// ==================== Étapes du Tick ====================
void AKartPawn::UpdateGround(float DeltaTime)
{
    // Un rayon sous chaque roue : la hauteur et la pente viennent des roues, pas de la sphère
    // (chaque roue glisse sur le terrain sans à-coup, alors que la sphère accroche les petites bosses)
    FVector GroundPoint;
    FVector WheelsNormal;
    const int32 WheelsOnFloor = TraceGroundUnderWheels(GroundPoint, WheelsNormal);
    const bool bHitFloor = WheelsOnFloor > 0;
    // Hauteur du centre du kart au-dessus du sol
    const float HeightAboveGround = FVector::DotProduct(GetActorLocation() - GroundPoint, GetActorUpVector());
    // Si on était déjà au sol, on y reste collé d'un peu plus loin pour ne pas décoller en haut des bosses
    const float MaxGroundHeight = RideHeight + (bIsGrounded ? GroundStickDistance : GroundSnapTolerance);
    bIsGrounded = bHitFloor && HeightAboveGround <= MaxGroundHeight && VerticalSpeed <= 0.f;
    // Si on voit un sol (même un peu loin), on s'aligne dessus, sinon on se remet droit
    GroundNormal = bHitFloor ? WheelsNormal : FVector::UpVector;
    if (bIsGrounded)
    {
        // Au sol on ne tombe plus
        VerticalSpeed = 0.f;
        // On recolle le kart à la bonne hauteur au-dessus du sol (vers le haut ou vers le bas)
        const float HeightError = HeightAboveGround - RideHeight;
        AddActorWorldOffset(-GetActorUpVector() * HeightError, true);
    }
    else
    {
        // En l'air la gravité nous fait accélérer vers le bas
        VerticalSpeed -= GravityStrength * DeltaTime;
    }
}
void AKartPawn::UpdateSpeed(float DeltaTime)
{
    // Pendant un boost, on fonce à la vitesse du boost, même sans accélérer
    if (BoostTimeRemaining > 0.f)
    {
        CurrentSpeed = BoostSpeed;
        return;
    }
    // En l'air on garde sa vitesse
    if (!bIsGrounded)
    {
        return;
    }
    float TargetSpeed = 0.f;
    float Rate = CoastDeceleration;
    if (ThrottleInput > 0.f)
    {
        // On accélère vers l'avant (on freine d'abord si on reculait)
        TargetSpeed = GetCurrentMaxSpeed() * ThrottleInput;
        Rate = CurrentSpeed < 0.f ? BrakeDeceleration : Acceleration;
    }
    else if (ThrottleInput < 0.f)
    {
        // On freine, puis on recule
        TargetSpeed = MaxReverseSpeed * ThrottleInput;
        Rate = CurrentSpeed > 0.f ? BrakeDeceleration : Acceleration;
    }
    // On rapproche la vitesse de la cible, au maximum de "Rate" par seconde
    // FInterpConstantTo avance d'un pas CONSTANT vers la cible (accélération régulière, comme un vrai moteur)
    // et s'arrête pile sur la cible sans la dépasser
    // (FInterpTo, utilisé plus bas pour la caméra, ralentit en approchant de la cible : bien pour un mouvement doux)
    CurrentSpeed = FMath::FInterpConstantTo(CurrentSpeed, TargetSpeed, DeltaTime, Rate);
}
void AKartPawn::UpdateSteering(float DeltaTime)
{
    // Pas de virage en l'air ou à l'arrêt
    if (!bIsGrounded || FMath::Abs(CurrentSpeed) < MinSpeedToTurn)
    {
        return;
    }
    float Steer = SteerInput;
    float Rate = TurnRate;
    if (bIsDrifting)
    {
        // En drift on tourne toujours du côté du drift :
        // le joystick dans le même sens serre le virage, dans l'autre sens il l'élargit
        const float SteerAlongDrift = SteerInput * DriftDirection;
        // GetMappedRangeValueClamped convertit une valeur d'une plage vers une autre :
        // ici -1..1 (joystick) devient 0.5..1 (force du virage), donc -1 -> 0.5, 0 -> 0.75, 1 -> 1
        // "Clamped" = si la valeur sort de -1..1, le résultat reste bloqué entre 0.5 et 1
        const float DriftSteer = DriftDirection * FMath::GetMappedRangeValueClamped(FVector2D(-1.f, 1.f), FVector2D(0.5f, 1.f), SteerAlongDrift);
        // On passe progressivement du virage normal au virage de drift (sinon le kart pivote d'un coup)
        // SmoothStep transforme 0..1 en une courbe en "S" : démarre doucement, accélère, puis finit doucement
        // Lerp(A, B, Alpha) mélange A et B : Alpha 0 = A, Alpha 1 = B, Alpha 0.5 = moitié-moitié
        const float Blend = FMath::SmoothStep(0.f, 1.f, DriftBlend);
        Steer = FMath::Lerp(SteerInput, DriftSteer, Blend);
        Rate *= FMath::Lerp(1.f, DriftTurnMultiplier, Blend);
    }
    // En marche arrière la direction est inversée, comme une vraie voiture
    const float MoveDirection = CurrentSpeed >= 0.f ? 1.f : -1.f;
    const float YawDelta = Steer * Rate * MoveDirection * DeltaTime;
    AddActorLocalRotation(FRotator(0.f, YawDelta, 0.f));
}
void AKartPawn::UpdateDrift(float DeltaTime)
{
    if (!bIsDrifting)
    {
        return;
    }
    // Trop lent : le drift s'arrête sans boost
    if (CurrentSpeed < DriftMinSpeed * 0.5f)
    {
        bIsDrifting = false;
        DriftDirection = 0;
        DriftChargeTime = 0.f;
        return;
    }
    // L'entrée dans le drift est progressive
    DriftBlend = FMath::Min(1.f, DriftBlend + DeltaTime / DriftEnterTime);
    // Le drift se charge seulement au sol
    if (bIsGrounded)
    {
        DriftChargeTime += DeltaTime;
    }
}
void AKartPawn::UpdateBoost(float DeltaTime)
{
    BoostTimeRemaining = FMath::Max(0.f, BoostTimeRemaining - DeltaTime);
}
void AKartPawn::MoveKart(float DeltaTime)
{
    // Au sol on avance le long de la pente, même si le kart n'a pas encore fini de s'incliner
    // (sinon il décolle en haut des bosses et tape en bas des montées)
    FVector Forward = GetActorForwardVector();
    if (bIsGrounded)
    {
        // VectorPlaneProject "aplatit" la direction sur le sol : on enlève la partie qui rentre dans le sol ou qui en sort
        // GetSafeNormal remet la longueur du vecteur à 1 (une direction pure), sans planter si le vecteur est nul
        Forward = FVector::VectorPlaneProject(Forward, GroundNormal).GetSafeNormal();
    }
    // Déplacement = vers l'avant + vers le haut/bas (gravité)
    const FVector Delta = Forward * CurrentSpeed * DeltaTime
        + FVector::UpVector * VerticalSpeed * DeltaTime;
    // "Sweep" = la sphère s'arrête si elle touche quelque chose au lieu de traverser
    FHitResult Hit;
    AddActorWorldOffset(Delta, true, &Hit);
    if (!Hit.bBlockingHit)
    {
        return;
    }
    // Si la sphère était déjà collée à quelque chose au départ, ce n'est pas un vrai choc
    const bool bHitWall = Hit.Normal.Z < WalkableFloorZ && !Hit.bStartPenetrating;
    if (bHitWall)
    {
        // 0 = on frôle le mur, 1 = on le prend de face
        // Sign donne le signe d'un nombre : +1 s'il est positif, -1 s'il est négatif, 0 s'il vaut 0
        // En marche arrière CurrentSpeed est négative : la direction du mouvement devient l'arrière du kart
        // (on le choisit car il gère avant / arrière / arrêt en une ligne, sans "if")
        const FVector MoveDirection = Forward * FMath::Sign(CurrentSpeed);
        const float ImpactStrength = FMath::Clamp(-FVector::DotProduct(MoveDirection, Hit.Normal), 0.f, 1.f);
        // On perd plus de vitesse quand on tape le mur de face
        CurrentSpeed *= FMath::Lerp(1.f, WallSpeedFactor, ImpactStrength);
    }
    else if (bIsGrounded && FVector::DotProduct(Hit.Normal, GroundNormal) >= WalkableFloorZ)
    {
        // Au sol, ce sont les roues qui gèrent la hauteur : la sphère ne doit jamais être freinée par le sol
        // (sinon elle s'arrête net à chaque petite bosse et le kart saccade)
        // On finit le mouvement sans collision, UpdateGround remettra le kart à la bonne hauteur
        ++SphereFloorHitCount;
        AddActorWorldOffset(Delta * (1.f - Hit.Time), false);
        return;
    }
    else if (VerticalSpeed < 0.f)
    {
        // On vient d'atterrir
        VerticalSpeed = 0.f;
    }
    // Le reste du mouvement glisse le long de la surface touchée
    const FVector RemainingDelta = FVector::VectorPlaneProject(Delta * (1.f - Hit.Time), Hit.Normal);
    AddActorWorldOffset(RemainingDelta, true);
}
void AKartPawn::AlignToGround(float DeltaTime)
{
    // Rotation cible : le haut du kart = la normale du sol, en gardant la même direction avant
    // MakeFromZX construit une rotation à partir de 2 axes : Z (le haut) en priorité, puis X (l'avant) au plus proche
    // FQuat (quaternion) = une autre façon d'écrire une rotation, sans les bugs des angles (blocage à 90°, etc.)
    const FQuat TargetRotation = FRotationMatrix::MakeFromZX(GroundNormal, GetActorForwardVector()).ToQuat();
    // On tourne petit à petit vers la cible (Slerp = interpolation de rotation)
    const float Alpha = FMath::Clamp(GroundAlignSpeed * DeltaTime, 0.f, 1.f);
    const FQuat NewRotation = FQuat::Slerp(GetActorQuat(), TargetRotation, Alpha);
    SetActorRotation(NewRotation);
}
void AKartPawn::UpdateVisuals(float DeltaTime)
{

    WheelSpinAngle += FMath::RadiansToDegrees(CurrentSpeed * DeltaTime / WheelRadius);

    WheelSpinAngle = FMath::Fmod(WheelSpinAngle, 360.f);

    const float WheelSteerAngle = SteerInput * MaxWheelSteerAngle;
    SetWheelRotation(WheelFrontLeft, WheelFrontLeftBaseRotation, WheelSteerAngle);
    SetWheelRotation(WheelFrontRight, WheelFrontRightBaseRotation, WheelSteerAngle);
    SetWheelRotation(WheelRearLeft, WheelRearLeftBaseRotation, 0.f);
    SetWheelRotation(WheelRearRight, WheelRearRightBaseRotation, 0.f);

    const float TargetSlideYaw = bIsDrifting ? DriftDirection * DriftSlideAngle * FMath::SmoothStep(0.f, 1.f, DriftBlend) : 0.f;

    BodySlideYaw = FMath::FInterpTo(BodySlideYaw, TargetSlideYaw, DeltaTime, DriftSlideInterpSpeed);

    FRotator BodyRotation = BodyBaseRotation;
    BodyRotation.Yaw += BodySlideYaw + SpinOutYaw;
    BodyMesh->SetRelativeRotation(BodyRotation);

    CameraSlideYaw = FMath::FInterpTo(CameraSlideYaw, BodySlideYaw * CameraDriftFollow, DeltaTime, CameraDriftInterpSpeed);

    const float KartPitch = GetActorRotation().Pitch;
    const float TargetSlopePitch = FMath::Abs(KartPitch) > CameraSlopeDeadZone ? KartPitch - FMath::Sign(KartPitch) * CameraSlopeDeadZone : 0.f;
    CameraSlopePitch = FMath::FInterpTo(CameraSlopePitch, TargetSlopePitch, DeltaTime, CameraSlopeFollowSpeed);
    FRotator ArmRotation = SpringArmBaseRotation;
    ArmRotation.Yaw += CameraSlideYaw;
    ArmRotation.Pitch += CameraSlopePitch;
    SpringArm->SetRelativeRotation(ArmRotation);
}
void AKartPawn::UpdateCameraFieldOfView(float DeltaTime)
{
    float TargetFieldOfView = BaseFieldOfView;
    if (BoostTimeRemaining > 0.f)
    {
        TargetFieldOfView = BoostFieldOfView;
    }
    else
    {
        // 0 à l'arrêt, 1 à vitesse max (seulement en marche avant)
        const float SpeedRatio = FMath::Clamp(CurrentSpeed / MaxForwardSpeed, 0.f, 1.f);
        TargetFieldOfView = FMath::Lerp(BaseFieldOfView, MaxSpeedFieldOfView, SpeedRatio);
    }
    // On change le FOV en douceur pour éviter les à-coups
    const float NewFieldOfView = FMath::FInterpTo(Camera->FieldOfView, TargetFieldOfView, DeltaTime, FieldOfViewInterpSpeed);
    Camera->SetFieldOfView(NewFieldOfView);
}
int32 AKartPawn::TraceGroundUnderWheels(FVector& OutGroundPoint, FVector& OutGroundNormal) const
{
    const FVector Up = GetActorUpVector();
    const FVector Forward = GetActorForwardVector() * GroundProbeHalfLength;
    const FVector Right = GetActorRightVector() * GroundProbeHalfWidth;
    // Un rayon sous chaque roue : avant gauche, avant droite, arrière gauche, arrière droite
    // (on part au-dessus du kart pour ne pas commencer sous le sol dans une montée)
    const FVector WheelOffsets[4] = { Forward - Right, Forward + Right, -Forward - Right, -Forward + Right };
    // FCollisionQueryParams = les options d'un rayon (trace) : qui ignorer, précision de collision, etc.
    // On s'en sert ici pour ignorer le kart lui-même : sans ça, le rayon partirait de l'intérieur de la sphère
    // et toucherait notre propre kart au lieu du sol
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    FVector Points[4];
    FVector PointSum = FVector::ZeroVector;
    FVector NormalSum = FVector::ZeroVector;
    int32 WheelsOnFloor = 0;
    for (int32 Index = 0; Index     < 4; ++Index)
    {
        const FVector Start = GetActorLocation() + WheelOffsets[Index] + Up * RideHeight;
        const FVector End = Start - Up * (RideHeight * 2.f + GroundTraceDistance);
        // LineTraceSingleByChannel = lance un rayon de Start à End et renvoie true s'il touche quelque chose
        // "Single" = on ne garde que le premier objet touché (le sol le plus proche), c'est tout ce qu'il nous faut
        // ECC_Visibility = le canal de collision "visibilité" : touche les objets visibles (sol, décor), ignore les triggers
        // Hit est rempli avec les infos du contact : point touché (ImpactPoint), pente de la surface (ImpactNormal)...
        // Une surface trop raide n'est pas un sol (c'est un mur)
        FHitResult Hit;
        const bool bHitFloor = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)
            && FVector::DotProduct(Hit.ImpactNormal, Up) >= WalkableFloorZ;
        if (bHitFloor)
        {
            Points[Index] = Hit.ImpactPoint;
            PointSum += Hit.ImpactPoint;
            NormalSum += Hit.ImpactNormal;
            ++WheelsOnFloor;
        }    
    }
    if (WheelsOnFloor == 0)
    {
        OutGroundPoint = GetActorLocation();
        OutGroundNormal = FVector::UpVector;
        return 0;
    }
    // Le sol est à la hauteur moyenne des roues
    OutGroundPoint = PointSum / WheelsOnFloor;
    // Certaines roues dans le vide (bord d'un saut) : on prend la pente moyenne sous les autres
    OutGroundNormal = NormalSum.GetSafeNormal();
    if (WheelsOnFloor == 4)
    {
        // Les deux diagonales du kart forment le plan du sol : leur produit vectoriel donne la pente
        // (ça suit la forme du terrain, même quand chaque roue est sur un triangle différent)
        const FVector DiagonalA = Points[0] - Points[3];
        const FVector DiagonalB = Points[1] - Points[2];
        FVector PlaneNormal = FVector::CrossProduct(DiagonalA, DiagonalB).GetSafeNormal();
        // On s'assure que la normale pointe vers le haut du kart
        if (FVector::DotProduct(PlaneNormal, Up) < 0.f)
        {
            PlaneNormal = -PlaneNormal;
        }
        if (!PlaneNormal.IsNearlyZero())
        {
            OutGroundNormal = PlaneNormal;
        }
    }
    return WheelsOnFloor;
}
void AKartPawn::SetWheelRotation(UStaticMeshComponent* Wheel, const FRotator& BaseRotation, float SteerAngle) const
{

    const FQuat SpinAndSteer = FQuat(FRotator(0.f, SteerAngle, WheelSpinAngle));
    Wheel->SetRelativeRotation(SpinAndSteer * FQuat(BaseRotation));
}
// ==================== Saut ====================
void AKartPawn::Jump()
{

    if (!bIsGrounded)
    {
        return;
    }

    if (!bCanMove || IsSpinningOut())
    {
        return;
    }

    VerticalSpeed = JumpVelocity;
    bIsGrounded = false;
}
// ==================== Drift et boost ====================
void AKartPawn::StartDrift()
{
    // Il faut être au sol, assez rapide, et tourner d'un côté
    if (!bIsGrounded || CurrentSpeed < DriftMinSpeed || FMath::IsNearlyZero(SteerInput))
    {
        return;
    }
    // Pas de drift pendant le compte à rebours ni pendant un tête-à-queue
    if (!bCanMove || IsSpinningOut())
    {
        return;
    }
    bIsDrifting = true;
    DriftDirection = SteerInput > 0.f ? 1 : -1;
    DriftChargeTime = 0.f;
    DriftBlend = 0.f;
}
void AKartPawn::EndDrift()
{
    if (!bIsDrifting)
    {
        return;
    }
    // Plus on a drifté longtemps, plus le boost est long
    if (DriftChargeTime >= SuperTurboChargeTime)
    {
        StartBoost(SuperTurboDuration);
    }
    else if (DriftChargeTime >= MiniTurboChargeTime)
    {
        StartBoost(MiniTurboDuration);
    }
    bIsDrifting = false;
    DriftDirection = 0;
    DriftChargeTime = 0.f;
}
void AKartPawn::StartBoost(float Duration)
{
    BoostTimeRemaining = Duration;
    CurrentSpeed = BoostSpeed;
}
float AKartPawn::GetCurrentMaxSpeed() const
{
    return BoostTimeRemaining > 0.f ? BoostSpeed : MaxForwardSpeed;
}
void AKartPawn::ShowDebugInfo() const
{
    if (GEngine == nullptr)
    {
        return;
    }
    
    FColor DriftColor = FColor::White;
    if (DriftChargeTime >= SuperTurboChargeTime)
    {
        DriftColor = FColor::Orange;
    }
    else if (DriftChargeTime >= MiniTurboChargeTime)
    {
        DriftColor = FColor::Cyan;
    }
    // Une clé différente par ligne pour que les messages se remplacent au lieu de s'empiler
    GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, FString::Printf(TEXT("Speed: %.0f  Grounded: %s"), 
        CurrentSpeed, bIsGrounded ? TEXT("yes") : TEXT("no")));
    GEngine->AddOnScreenDebugMessage(2, 0.f, DriftColor, FString::Printf(TEXT("Drift: %s  Charge: %.2f"), 
        bIsDrifting ? TEXT("yes") : TEXT("no"), DriftChargeTime));
    GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Yellow, FString::Printf(TEXT("Boost: %.2f"), 
        BoostTimeRemaining));
}
// ==================== Objets bonus ====================
void AKartPawn::ApplyBoost(float Duration)
{
    if (Duration > 0.f)
    {
        StartBoost(Duration);
    }
}
void AKartPawn::SpinOut(float Duration)
{

    if (Duration <= 0.f || IsSpinningOut())
    {
        return;
    }
    SpinOutTotalTime = Duration;
    SpinOutTimeRemaining = Duration;

    CurrentSpeed *= SpinOutSpeedFactor;
    BoostTimeRemaining = 0.f;
    bIsDrifting = false;
    DriftDirection = 0;
    DriftChargeTime = 0.f;
}
void AKartPawn::UpdateSpinOut(float DeltaTime)
{
    if (!IsSpinningOut())
    {
        SpinOutYaw = 0.f;
        return;
    }
    SpinOutTimeRemaining = FMath::Max(0.f, SpinOutTimeRemaining - DeltaTime);

    ThrottleInput = 0.f;
    SteerInput = 0.f;

    const float Progress = 1.f - SpinOutTimeRemaining / SpinOutTotalTime;

    const float EaseOut = 1.f - FMath::Square(1.f - Progress);
    SpinOutYaw = 360.f * SpinOutTurns * EaseOut;
}