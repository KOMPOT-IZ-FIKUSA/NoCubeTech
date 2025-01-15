// Fill out your copyright notice in the Description page of Project Settings.


#include "AAdditionalObject.h"

// Sets default values
AAdditionalObject::AAdditionalObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	globalWorldGenerator = nullptr;

}

// Called when the game starts or when spawned
void AAdditionalObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAdditionalObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

