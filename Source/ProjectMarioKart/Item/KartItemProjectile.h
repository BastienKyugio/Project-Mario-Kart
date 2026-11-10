#pragma once

#include "CoreMinimal.h"
#include "Item/KartItemHazard.h"
#include "KartItemProjectile.generated.h"

UCLASS()
class PROJECTMARIOKART_API AKartItemProjectile : public AKartItemHazard
{
	GENERATED_BODY()
	
public:
	AKartItemProjectile();
	
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=  "Kart|Projectile")
	float Speed = 4000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=  "Kart|Projectile", meta = (ClampMin = "0"))
	int32 MaxBounces = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=  "Kart|Projectile")
	float GravityStrength = 2500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=  "Kart|Projectile")
	float GroundStickDistance = 30.f;
	
private:
	FVector MoveDirection = FVector::ForwardVector;
	
	float VerticalSpeed = 0.f;
	
	int32 BouncesLeft = 0;
	
	
};