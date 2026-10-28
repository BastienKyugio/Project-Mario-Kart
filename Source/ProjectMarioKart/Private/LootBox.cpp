#include "LootBox.h"

#include "KartPawn.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"


ALootBox::ALootBox()
{

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;                           
	Collision->InitSphereRadius(60.f);                   
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	Mesh->SetupAttachment(RootComponent);              
	//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ModeleCylindre(TEXT("/Game/Meshes/SM_RearWheel_Left"));
	if (ModeleCylindre.Succeeded())
	{
		Mesh->SetStaticMesh(ModeleCylindre.Object);
	}
	
	Mesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.1f));
	Mesh->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	
	RotationSpeed = 180.f;
	bIsFloating = true;
	FloatingHeight = 20.f;
	FloatingSpeed = 3.f;
}

void ALootBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	AKartPawn*  Kart = Cast<AKartPawn>(OtherActor);

	if (Kart)
	{
		Destroy();
	}
}
