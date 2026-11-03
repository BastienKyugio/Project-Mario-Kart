// Fill out your copyright notice in the Description page of Project Settings.


#include "World/RotatingObject.h"
#include "Components/SceneComponent.h"


// Sets default values
ARotatingObject::ARotatingObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	
	AnimatedRoot = CreateDefaultSubobject<USceneComponent>("AnimatedRoot");
	AnimatedRoot->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ARotatingObject::BeginPlay()
{
	Super::BeginPlay();
	
	AnimatedRootBaseLocation = AnimatedRoot->GetRelativeLocation();
	
	if (bRandomStartPhase)
	{
		AnimationTime = FMath::FRandRange(0.f, 1.f / BobFrequency);
		AnimatedRoot->AddLocalRotation(RotationSpeed * AnimationTime);
	}
}

// Called every frame
void ARotatingObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bAnimate)
	{
		return;
	}
	
	AnimationTime += DeltaTime;
	
	AnimatedRoot->AddLocalRotation(RotationSpeed * DeltaTime);
	
	if (bEnableBobbing)
	{
		const float BobOffset = FMath::Sin(AnimationTime * BobFrequency * 2.f * PI) * BobHeight;
		AnimatedRoot->SetRelativeLocation(AnimatedRootBaseLocation + FVector(0.f, 0., BobOffset));
	}
}

void ARotatingObject::SetAnimationEnabled(bool bEnable)
{
	bAnimate = bEnable;
}
