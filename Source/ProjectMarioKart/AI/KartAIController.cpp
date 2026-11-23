#include "AI/KartAIController.h"

#include "AI/KartDriverProfile.h"
#include "KartPawn.h"
#include "Track/KartRaceTrack.h"

AKartAIController::AKartAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AKartAIController::BeginPlay()
{
	Super::BeginPlay();
	
	Track = AKartRaceTrack::Find(this);
	if (Track == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KartAIController : aucun AKartRaceTrack dans le niveau, l'IA ne sait pas où aller"));
	}
	WanderTime = FMath::FRandRange(0.f, 100.f);
}

void AKartAIController::SetDriverProfile(UKartDriverProfile* InProfile)
{
	Profile = InProfile;
}

const UKartDriverProfile* AKartAIController::GetProfileOrDefault() const
{
	return Profile !=nullptr ? Profile.Get() :: GetDefault<UKartDriverProfile>();
}
