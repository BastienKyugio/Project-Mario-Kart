#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingObject.generated.h"

class UStaticMeshComponent;

UCLASS()
class PROJECTMARIOKART_API ARotatingObject : public AActor
{
	GENERATED_BODY()
	
public:	
	ARotatingObject();
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Rotating")
	void SetAnimationEnabled(bool bEnable);
	
	UFUNCTION(BlueprintCallable, Category = "Rotating")
	bool IsAnimationEnabled() const { return bAnimate; };
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotating|Components")
	TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotating|Components")
	TObjectPtr<USceneComponent> AnimatedRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating|Rotation")
	FRotator RotationSpeed = FRotator(30.f, 90.0f, 45.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating|Bobbing")
	bool bEnableBobbing = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating|Bobbing", meta = (ClampMin = "0"))
	float BobHeight = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating|Bobbing", meta = (ClampMin = "0.01"))
	float BobFrequency = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating")
	bool bRandomStartPhase = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotating")
	bool bAnimate = true;
	
	
private:
	float AnimationTime = 0.f;
	FVector AnimatedRootBaseLocation = FVector::ZeroVector;
};	