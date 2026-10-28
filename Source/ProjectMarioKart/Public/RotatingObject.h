// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ARotatingObject();
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement");
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement");
	float RotationSpeed = 90.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement");
	bool bIsFloating = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement");
	float FloatingHeight = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement");
	float FloatingSpeed = 2.f;
	
	
	
private:
	FVector StartPosition;
	float RemainingTime = 0.f;
};	