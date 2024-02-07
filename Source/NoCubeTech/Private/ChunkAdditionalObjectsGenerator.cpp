// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkAdditionalObjectsGenerator.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChunkAdditionalObjectsGenerator::AChunkAdditionalObjectsGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	objectsRegisteredToCells = FActorListGrid();
	additionalObjectsGenerated = false;
}

// Called when the game starts or when spawned
void AChunkAdditionalObjectsGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (!tryFindGeneratorIfNecessary()) {
		return;
	}
}

// Called every frame
void AChunkAdditionalObjectsGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!tryFindGeneratorIfNecessary()) {
		return;
	}
	tryGenerateObjectsIfNecessary();

}

bool AChunkAdditionalObjectsGenerator::tryFindGeneratorIfNecessary() {
	if (worldGenerator.IsValid() && worldGenerator->GetGenerator()) {
		return true;
	}
	AActor* worldGenerator_ = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalWorldGenerator::StaticClass());
	if (worldGenerator_) {
		AGlobalWorldGenerator* generatorActor = ((AGlobalWorldGenerator*)worldGenerator_);
		worldGenerator = generatorActor;
		return true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot find world generator"));
		UE_LOG(LogTemp, Warning, TEXT("AChunkAdditionalObjectsGenerator : Cannot find world generator"))
			return false;
	}
}

void AChunkAdditionalObjectsGenerator::tryGenerateObjectsIfNecessary() {
	if (additionalObjectsGenerated || !worldGenerator.IsValid() || !worldGenerator.Get())
		return;
	const WorldGenerator& generator = *worldGenerator->GetGenerator();

	if (chunkX == 3 && chunkY == 0) {
		FString str = TEXT("");
		str.Appendf(TEXT("Generating objects for chunk %d, %d"), chunkX, chunkY);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, str);
	}
	additionalObjectsGenerated = true;
}