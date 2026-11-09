#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KartItemHazard.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
class AKartPawn;
struct FHitResult;

UCLASS()
class PROJECTMARIOKART_API AKartItemHazard : public AActor
{
	GENERATED_BODY()
	
public:
	AKartItemHazard();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Kart|Components")
	TObjectPtr<USphereComponent> CollisionSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Kart|Components")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Hazard", meta = (ClampMin = "0.1"))
	float SpinOutDuration = 1.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Hazard", meta = (ClampMin = "0"))
	float HoverHeight = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Kart|Hazard", meta = (ClampMin = "0"))
	float OwnerSafeTime = 0.5f;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Kart|Hazard")
	void OnHitKart(AKartPawn* Kart);
	
	bool FindGround(float MaxDistance, FHitResult& OutHit) const;
	
private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
