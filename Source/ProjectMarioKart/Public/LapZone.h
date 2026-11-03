// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LapZone.generated.h"
class UBoxComponent;
class UArrowComponent;
class UPrimitiveComponent;
struct FHitResult;


UCLASS()
class PROJECTMARIOKART_API ALapZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALapZone();
	
	UFUNCTION(BlueprintPure, Category = "Kart|Lap")
	int32 GetZoneIndex() const { return ZoneIndex; }
	
	UFUNCTION(BlueprintPure, Category = "Kart|Lap")
	bool IsGoingRightWay(const FVector& MoveDirection) const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UBoxComponent> Box;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Kart|Components")
	TObjectPtr<UArrowComponent> Arrow;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Kart|Lap", meta = (ClampMin = "0"))
	int32 ZoneIndex = 0;
	
	UPROPERTY(EditAnywhere, Category = "Kart|Lap")
	bool bShowInGame = true;

private:
	UFUNCTION()
	void HandleBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
