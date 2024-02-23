// Fill out your copyright notice in the Description page of Project Settings.


#include "TestObjectGenerator.h"
#include "ChunkSavableActor.h"

// Sets default values
ATestObjectGenerator::ATestObjectGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestObjectGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATestObjectGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (create) {

		AGlobalChunkRegistry* globalChunkRegistry = Cast<AGlobalChunkRegistry>(UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalChunkRegistry::StaticClass()));
		
		check(globalChunkRegistry->PositionToIndex(0) == 0);
		check(globalChunkRegistry->PositionToIndex(-3200) == 0);
		check(globalChunkRegistry->PositionToIndex(3199) == 0);
		check(globalChunkRegistry->PositionToIndex(3200) == 1);

		FActorSpawnParameters parameters = FActorSpawnParameters();
		parameters.Name = FName("TestActorWithNoName");
		FTransform transform = FTransform::Identity;
		AActor* spawnedActor = GetWorld()->SpawnActor(AChunkSavableActor::StaticClass(), &transform, parameters);
		AChunkSavableActor* spawnedActor1 = Cast<AChunkSavableActor>(spawnedActor);
		spawnedActor1->SetupByDefault(globalChunkRegistry);
		
	}
	create = false;
}

