// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkAdditionalObjectsGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "AbstractAdditionalObjGenerator.h"
#include "AdditionalObjectsGeneratorRegistry.h"
#include "ChunkAnchorActor.h"
#include "BiomesConst.h"

// Sets default values
AChunkAdditionalObjectsGenerator::AChunkAdditionalObjectsGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	additionalObjectsGenerated = false;
	SetReplicates(false);

	generatedLandscapePositions = TArray<FPosition2DWithId>();

	SetActorTickInterval(0.2);
}

bool AChunkAdditionalObjectsGenerator::HasFinishedLandscapeObjectsGeneration() {
	/*
	switch (generationState) {
	case LANDSCAPE_GENERATION:
	case LANDSCAPE_VALIDATION:
		return false;
	case WAITING_FOR_NEIGHBORS:
	case OBJECTS_GENERATION:
	case OBJECTS_VALIDATION:
	case DONE:
		return true;
	default:
		check(false);
		return false;
	}
	*/
	return false;
}



void AChunkAdditionalObjectsGenerator::handleGenerationIteration() {
	int i = 0;
	while (generatedLandscapePositions.Num() > 0 && i < MaximumNewObjectsPerTick) {
		int lastIndex = generatedLandscapePositions.Num() - 1;
		FPosition2DWithId positionAndGeneratorId = generatedLandscapePositions[lastIndex];
		generatedLandscapePositions.RemoveAt(lastIndex);
		
		WorldGenerator& generator = *globalWorldGenerator->GetGenerator();
		AbstractAdditionalObjGenerator* objectGenerator = generator.additionalObjectsGeneratorRegistry->GetGeneratorById(positionAndGeneratorId.id);
		FVector2D pos = positionAndGeneratorId.position;
		objectGenerator->CreateActor(GetWorld(), getChunkRegistry(), generator, pos.X, pos.Y, this);

		i++;
	}
}

// Called every frame
void AChunkAdditionalObjectsGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Error: ticking a server-only AChunkAdditionalObjectsGenerator without authority"));
		return;
	}

	//updateFoundNeighbors();
	handleGenerationIteration();


}

bool AChunkAdditionalObjectsGenerator::tryFindGeneratorIfNecessary() {
	if (globalWorldGenerator.IsValid() && globalWorldGenerator->GetGenerator()) {
		return true;
	}
	AActor* worldGenerator_ = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalWorldGenerator::StaticClass());
	if (worldGenerator_) {
		AGlobalWorldGenerator* generatorActor = ((AGlobalWorldGenerator*)worldGenerator_);
		globalWorldGenerator = generatorActor;
		return true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot find world generator"));
		UE_LOG(LogTemp, Warning, TEXT("AChunkAdditionalObjectsGenerator : Cannot find world generator"))
			return false;
	}
}

void AChunkAdditionalObjectsGenerator::extractBiomesIdsFromProbabilities(const BiomeWeights& probabilities, TSet<int>& result) {
	for (int i = 0; i < BIOMES_COUNT; i++) {
		if (probabilities.values[i] > 0.01) {
			result.Add(i);
		}
	}
}

TSet<int> AChunkAdditionalObjectsGenerator::FindBiomesIds(const WorldGenerator& generator) {
	const AGlobalChunkRegistry& chunkRegistry_ = *getChunkRegistry();
	TSet<int> foundBiomes = TSet<int>();
	FVector selfLocation = GetActorLocation();
	float chunkSizeHalf = chunkRegistry_.WorldPartitionCellSize / 2;

	BiomeWeights weights = generator.GenerateBiomeWeightsForAdditionalObjects(selfLocation.X - chunkSizeHalf, selfLocation.Y - chunkSizeHalf);
	extractBiomesIdsFromProbabilities(weights, foundBiomes);

	weights = generator.GenerateBiomeWeightsForAdditionalObjects(selfLocation.X - chunkSizeHalf, selfLocation.Y + chunkSizeHalf);
	extractBiomesIdsFromProbabilities(weights, foundBiomes);

	weights = generator.GenerateBiomeWeightsForAdditionalObjects(selfLocation.X + chunkSizeHalf, selfLocation.Y - chunkSizeHalf);
	extractBiomesIdsFromProbabilities(weights, foundBiomes);

	weights = generator.GenerateBiomeWeightsForAdditionalObjects(selfLocation.X + chunkSizeHalf, selfLocation.Y + chunkSizeHalf);
	extractBiomesIdsFromProbabilities(weights, foundBiomes);

	return foundBiomes;

}

void AChunkAdditionalObjectsGenerator::generatePositionsAndGeneratorsIds() {
	FVector selfLocation = GetActorLocation();
	if (!tryFindGeneratorIfNecessary()) {
		return;
	}
	WorldGenerator& worldGenerator = *globalWorldGenerator->GetGenerator();
	TSet<int> foundBiomes = FindBiomesIds(worldGenerator);
	TArray<FAdditionalGeneratorWithBiome> additionalObjectsGeneratorsForBiomes = worldGenerator.additionalObjectsGeneratorRegistry->GetGeneratorsSortedByPriority(foundBiomes);
	for (FAdditionalGeneratorWithBiome generatorAndBiome : additionalObjectsGeneratorsForBiomes) {
		int biomeId = generatorAndBiome.biome->GetId();
		AbstractAdditionalObjGenerator* objectGenerator = generatorAndBiome.generator;
		TArray<FVector2D> positionsToCheck = objectGenerator->GeneratePositionsForChunk(worldGenerator, selfLocation.X, selfLocation.Y, getChunkRegistry()->WorldPartitionCellSize);
		// For each position, check if the biome is probable
		for (FVector2D position : positionsToCheck) {
			BiomeWeights weights = worldGenerator.GenerateBiomeWeightsForAdditionalObjects(position.X, position.Y);
			float probabilityThreshold = weights.values[biomeId];
			int64 randomSeedForPoint = worldGenerator.seed + (int64)(position.X * 8071752727) + (int64)(position.Y * 7931403763);
			float randomValue = RandomGenerator::IntToFloat((int)(randomSeedForPoint % 100000));
			if (randomValue < probabilityThreshold) {
				generatedLandscapePositions.Add(FPosition2DWithId(position, objectGenerator->GetId()));
			}
		}
	}
}

void AChunkAdditionalObjectsGenerator::setupByDefault() {

	AGlobalChunkRegistry* cr = getChunkRegistry();
	FVector selfLocation = GetActorLocation();

	chunkX = cr->PositionToIndex(selfLocation.X);
	chunkY = cr->PositionToIndex(selfLocation.Y);

	generatePositionsAndGeneratorsIds();
}

void AChunkAdditionalObjectsGenerator::loadFromArchive(FArchive& ar) {
	if (!tryFindGeneratorIfNecessary()) {
		return;
	}
	// deserialize simple fields
	ar << chunkX;
	ar << chunkY;
}

void AChunkAdditionalObjectsGenerator::saveToArchive(FArchive& ar) {
	if (!tryFindGeneratorIfNecessary()) {
		return;
	}
	// Serialize simple fields
	ar << chunkX;
	ar << chunkY;

}