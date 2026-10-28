// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingObject.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"


// Sets default values
ARotatingObject::ARotatingObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	
	
}

// Called when the game starts or when spawned
void ARotatingObject::BeginPlay()
{
	Super::BeginPlay();
	
	StartPosition = GetActorLocation();
	
}

// Called every frame
void ARotatingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float AddAngle = RotationSpeed * DeltaTime;
	
	AddActorLocalRotation(FRotator(0.f, AddAngle, 0.f));
	
	if (bIsFloating)
	{
		RemainingTime = RemainingTime + DeltaTime;
		
		float Offset = FMath::Sin(RemainingTime * FloatingSpeed) * FloatingHeight;
		
		FVector NewPosition = StartPosition;
		NewPosition.Z = StartPosition.Z + Offset;
		SetActorLocation(NewPosition);
	}

}
