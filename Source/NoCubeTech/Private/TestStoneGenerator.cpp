// Fill out your copyright notice in the Description page of Project Se

#include "TestStoneGenerator.h"

// Sets default values
ATestStoneGenerator::ATestStoneGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	meshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(FName("ProceduralMeshComponent"));
	meshComponent->RegisterComponent();
	AddInstanceComponent(meshComponent);
	SetRootComponent(meshComponent);

}

// Called when the game starts or when spawned
void ATestStoneGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}



