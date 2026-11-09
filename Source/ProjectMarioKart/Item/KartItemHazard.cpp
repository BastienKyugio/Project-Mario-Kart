#include "KartItemHazard.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "KartPawn.h"

AKartItemHazard::AKartItemHazard()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetSphereRadius(40.f);
	RootComponent = CollisionSphere;
	
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CollisionSphere->SetGenerateOverlapEvents(true);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(CollisionSphere);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKartItemHazard::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AKartItemHazard::HandleBeginOverlap);
	
	FHitResult GroundHit;
	if (FindGround(1000.f, GroundHit))
	{
		SetActorLocation(GroundHit.ImpactPoint + FVector::UpVector * HoverHeight);
	}
}

bool AKartItemHazard::FindGround(float MaxDistance, FHitResult& OutHit) const
{
	const FVector Start = GetActorLocation() + FVector::UpVector * 50.f;
	const FVector End = GetActorLocation() - FVector::UpVector * MaxDistance;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	
	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
	
}

void AKartItemHazard::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKartPawn* Kart = Cast<AKartPawn>(OtherActor);
	if (Kart == nullptr)
	{
		return;
	}
	
	if (Kart == GetOwner() && GetGameTimeSinceCreation() < OwnerSafeTime)
	{
		return;
	}
	Kart->SpinOut(SpinOutDuration);
	OnHitKart(Kart);
	
	Destroy();
	
}
