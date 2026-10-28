#pragma once

#include "CoreMinimal.h"
#include "RotatingObject.h"
#include "LootBox.generated.h"

class USphereComponent;


UCLASS()
class ALootBox : public ARotatingObject
{
	GENERATED_BODY() 

public:

	ALootBox();

protected:
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* Collision;
};
