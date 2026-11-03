#include "Item/KartItemBox.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/KartItemComponent.h"
#include "TimerManager.h"

AKartItemBox::AKartItemBox()
{

    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(Root);
    DetectionSphere->SetSphereRadius(80.f);

    DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
    DetectionSphere->SetGenerateOverlapEvents(true);

    BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
    BoxMesh->SetupAttachment(AnimatedRoot);
    BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AKartItemBox::BeginPlay()
{

    Super::BeginPlay();
    DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AKartItemBox::HandleBeginOverlap);
}
void AKartItemBox::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    UKartItemComponent* ItemComponent = OtherActor != nullptr ? OtherActor->FindComponentByClass<UKartItemComponent>() : nullptr;
    if (ItemComponent == nullptr)
    {
        return;
    }

    ItemComponent->TryGiveRandomItem(ItemPool);
    SetBoxActive(false);
    OnBoxCollected(OtherActor);
    GetWorldTimerManager().SetTimer(RespawnTimer, this, &AKartItemBox::Respawn, RespawnDelay, false);
}
void AKartItemBox::SetBoxActive(bool bActive)
{

    BoxMesh->SetVisibility(bActive, true);

    DetectionSphere->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);

    SetAnimationEnabled(bActive);
}
void AKartItemBox::Respawn()
{
    SetBoxActive(true);
}