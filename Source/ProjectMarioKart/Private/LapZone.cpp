// Fill out your copyright notice in the Description page of Project Settings.


#include "LapZone.h"

// Sets default values
ALapZone::ALapZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALapZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALapZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

