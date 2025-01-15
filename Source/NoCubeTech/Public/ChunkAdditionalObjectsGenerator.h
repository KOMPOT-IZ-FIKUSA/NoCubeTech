// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataStructures.h"
#include "ChunkSavableActor.h"
#include "GlobalWorldGenerator.h"
#include "ChunkAdditionalObjectsGenerator.generated.h"


USTRUCT()
struct FPosition2DWithId {
	GENERATED_BODY()
public:
	FVector2D position;
	int id;

	FPosition2DWithId(const FVector2D& position, int id)
		: position(position), id(id)
	{
	}

	FPosition2DWithId() = default;
};

UCLASS()
class NOCUBETECH_API AChunkAdditionalObjectsGenerator : public AChunkSavableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkAdditionalObjectsGenerator();

protected:

	UPROPERTY()
	int chunkX;

	UPROPERTY()
	int chunkY;

	static const int MaximumNewObjectsPerTick = 1;


	TWeakObjectPtr<AGlobalWorldGenerator> globalWorldGenerator;

	UPROPERTY()
	TArray<FPosition2DWithId> generatedLandscapePositions;


	UPROPERTY()
	bool additionalObjectsGenerated;

	bool tryFindGeneratorIfNecessary();

	void setupByDefault() override;

	static void extractBiomesIdsFromProbabilities(const BiomeWeights& weights, TSet<int>& result);

	void loadFromArchive(FArchive& archive) override;
	
	void saveToArchive(FArchive& archive) override;


	void handleGenerationIteration();

	void generatePositionsAndGeneratorsIds();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TSet<int> FindBiomesIds(const WorldGenerator& generator);

	bool HasFinishedLandscapeObjectsGeneration();

};
