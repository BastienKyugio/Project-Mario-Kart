#include "Item/KartItemData_SpawnActor.h"
#include "Engine/World.h"
#include "KartPawn.h"

bool UKartItemData_SpawnActor::Activate(AKartPawn* Kart) const
{
	if (Kart == nullptr || ActorClass == nullptr)
	{
		return false;
	}
	
	const FVector SpawnLocation = Kart->GetActorTransform().TransformPosition(SpawnOffset);
	
	const FRotator SpawnRotation(0.f, Kart->GetActorRotation().Yaw, 0.f);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Kart;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	const AActor* SpawnedActor = Kart->GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLocation, SpawnRotation, SpawnParams);
	return SpawnedActor != nullptr;
}
