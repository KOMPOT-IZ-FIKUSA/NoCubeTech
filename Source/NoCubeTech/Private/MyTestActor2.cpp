// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTestActor2.h"

// Sets default values
AMyTestActor2::AMyTestActor2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DefaultSceneComponent")));
	SetRootComponent(rootSceneComponent);
}

// Called when the game starts or when spawned
void AMyTestActor2::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyTestActor2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyTestActor2::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

