// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KartPawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UInputAction;
class UCameraComponent;
class UInputMappingContext;
struct FInputActionValue;


UCLASS()
class PROJECTMARIOKART_API AKartPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AKartPawn();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable, Category = "Kart")
	void SetCanMove(bool bInCanMove);
	
	UFUNCTION(BlueprintPure, Category = "Kart")
	bool CanMove() const { return bCanMove; }
	
	UFUNCTION(BlueprintPure, Category = "Kart")
	FVector GetMoveDirection() const;
	
	void EnterLapZone(int32 Index);
	
	UFUNCTION(BlueprintCallable, Category = "Kart|Race")
	void CompleteLap();
	
	UFUNCTION(BlueprintCallable, Category = "Kart|Race")
	void ResetRaceProgress();
	
	UFUNCTION(BlueprintPure, Category = "Kart|Race")
	int32 GetCurrentLap() const { return CurrentLap; }
	
	UFUNCTION(BlueprintPure, Category = "Kart|Race")
	bool HasFinishedRace() const { return bHasFinishedRace; }
	
	UFUNCTION(BlueprintPure, Category = "Kart|Race")
	float GetFinishTime() const { return FinishTime; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void NotifyControllerChanged() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<USphereComponent> CollisionSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UStaticMeshComponent> WheelFrontLeft;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UStaticMeshComponent> WheelFrontRight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UStaticMeshComponent> WheelRearLeft;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UStaticMeshComponent> WheelRearRight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UCameraComponent>  Camera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Input")
	TObjectPtr<UInputMappingContext> KartMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Input")
	TObjectPtr<UInputAction> ThrottleAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Input")
	TObjectPtr<UInputAction> SteerAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Input")
	TObjectPtr<UInputAction> DriftAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float MaxForwardSpeed = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float MaxReverseSpeed = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float Acceleration = 1500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float BrakeDeceleration = 3000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float CoastDeceleration = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float TurnRate = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float MinSpeedToTurn = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement", meta = (ClampMin = "0", ClampMax = "1"))
	float WallSpeedFactor = 0.3f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float GravityStrength = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Movement")
	float GroundTraceDistance = 80.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Gravity")
	float RideHeight = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Gravity")
	float GroundStickDistance = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Gravity")
	float GroundAlignSpeed = 8.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Gravity")
	float GroundProbeHalfLength = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Gravity")
	float GroundProbeHalfWidth = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Jump")
	float JumpVelocity = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float DriftMinSpeed = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float DriftTurnMultiplier = 1.4f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float DriftSlideAngle = 25.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift", meta = (ClampMin = "0.01"))
	float DriftEnterTimer = 0.4f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float DriftSlideInterpSpeed = 6.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float MiniTurboChargeTime = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float SuperTurboChargeTime = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float MiniTurboDuration = 0.6f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float SuperTurboDuration = 1.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Drift")
	float BoostSpeed = 3500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Visual")
	float WheelRadius = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Visual")
	float MaxWheelSteerAngle = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera")
	float BaseFieldOfView = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera")
	float MaxSpeedFieldOfView = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera")
	float BoostFieldOfView = 110.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera", meta = (ClampMin = "0", ClampMax = "1"))
	float CameraDriftFollow = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera")
	float CameraDriftInterSpeed = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera", meta = (ClampMin = "0", ClampMax = "90"))
	float CameraSlopeDeadZone = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera")
	float CameraSlopeFollowSpeed = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Camera")
	float FieldOfViewInterpSpeed = 4.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Visual")
	bool bShowDebug = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float CurrentSpeed = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float VerticalSpeed = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float ThrottleInput = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float SteerInput = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	bool bIsGrounded = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	bool bIsDrifting = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	int32 DriftDirection = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float DriftChargeTime = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float DriftBlend = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float BoostTimeRemaining = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	float WheelSpinAngle = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	FVector GroundNormal = FVector::UpVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|State")
	bool bCanMove = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
	int32 CurrentLap = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
	bool bHasFinishedRace = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
	float FinishTime = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Race")
	int32 NextCheckpoint = 1;
	
private:
	// Fonction appelées par l'input
	void OnThrottle(const FInputActionValue& Value);
	void OnThrottleReleased(const FInputActionValue& Value);
	void OnSteer(const FInputActionValue& Value);
	void OnSteerReleased(const FInputActionValue& Value);
	void OnDriftPressed(const FInputActionValue& Value);
	void OnDriftReleased(const FInputActionValue& Value);
	void OnJumpPressed(const FInputActionValue& Value);
	
	// Étapes du Tick (dans l'ordre)
	
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
	
	int32 TraceGroundUnderWheel(FVector& OutGroundPoint, FVector& OutGroundNormal) const;
	
	//Saut
	void Jump();
	
	// Drift et Boost
	void StartDrift();
	void EndDrift();
	void StartBoost(float Duration);
	float GetCurrentMaxSpeed() const;
	
	//Visuel des roues
	void SetWheelRotation(UStaticMeshComponent* Wheel, const FRotator& BaseRotation, float SteerAngle) const;
	
	//Rotation de départ placées dans le bp ( on tourne "par dessus")
	FRotator BodyBaseRotation;
	FRotator SpringArmBaseRotation;
	FRotator WheelFrontLeftBaseRotation;
	FRotator WheelFrontRightBaseRotation;
	FRotator WheelRearLeftBaseRotation;
	FRotator WheelRearRightBaseRotation;
	
	float CameraSlideYaw = 0.f;
	
	float CameraSlopePitch = 0.f;
	
	int32 SphereFloorHitCount = 0;
};
