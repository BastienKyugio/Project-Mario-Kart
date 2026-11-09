#pragma once

#include "CoreMinimal.h"
#include "World/RotatingObject.h"
#include "KartItemBox.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UKartItemPoolData;
class UPrimitiveComponent;
struct FHitResult;

UCLASS()
class PROJECTMARIOKART_API AKartItemBox : public ARotatingObject
{
	GENERATED_BODY()
	
public:
	AKartItemBox();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<USphereComponent> DetectionSphere;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UStaticMeshComponent> BoxMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|ItemBox")
	TObjectPtr<UKartItemPoolData> ItemPool;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|ItemBox", meta = (ClampMin = "0.1"))
	float RespawnDelay = 3.f;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Kart|ItemBox")
	void OnBoxCollected(AActor* Collector);
	
private:
	
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetBoxActive(bool bActive);

	void Respawn();
	FTimerHandle RespawnTimer;
};