// Fill out your copyright notice in the Description page of Project Settings.


#include "LapZone.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "KartPawn.h"

// Sets default values
ALapZone::ALapZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Box = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	RootComponent = Box;
	Box->SetBoxExtent(FVector(50.0f, 500.0f, 200.0f));
	Box->SetCollisionProfileName(FName("Trigger"));
	Box->SetGenerateOverlapEvents(true);
	
	Arrow = CreateDefaultSubobject<UArrowComponent>(FName("Arrow"));
	Arrow->SetupAttachment(Box);
	
	Arrow->SetArrowColor(FLinearColor::Green);
	Arrow->SetArrowSize(3.f);
	Arrow->SetArrowLength(100.f);
}

// Called when the game starts or when spawned
void ALapZone::BeginPlay()
{
	Super::BeginPlay();
	Box->SetHiddenInGame(!bShowInGame);
	Arrow->SetHiddenInGame(!bShowInGame);
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &ALapZone::HandleBeginOverLap);
}

bool ALapZone::IsGoingRightWay(const FVector& MoveDirection) const
{
	return FVector::DotProduct(MoveDirection, Arrow->GetForwardVector()) > 0.f;
}

void ALapZone::HandleBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKartPawn* Kart = Cast<AKartPawn>(OtherActor);
	if (Kart == nullptr)
	{
		return;
	}
	
	if (!IsGoingRightWay(Kart->GetMoveDirection()))
	{
		return;
	}
	
	Kart->EnterLapZone(ZoneIndex);
}