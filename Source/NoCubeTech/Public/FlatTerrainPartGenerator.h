// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractAdditionalObjGenerator.h"
#include "WorldGenerator.h"
#include "FlatTerrainPart.h"

/**
 * 
 */
class NOCUBETECH_API FlatTerrainPartGenerator : public AbstractAdditionalObjGenerator
{
public:
	FlatTerrainPartGenerator();
	virtual ~FlatTerrainPartGenerator();



	AAdditionalObject* CreateActor(UWorld* world, AGlobalChunkRegistry* chunkRegistry, WorldGenerator& worldGenerator, float x, float y, UObject* outer) const override {
		FActorSpawnParameters parameters = FActorSpawnParameters();

		FVector location = FVector(x, y, worldGenerator.GenerateHeight(x, y));

		AFlatTerrainPart* result = world->SpawnActor<AFlatTerrainPart>(location, FRotator::ZeroRotator, parameters);
		result->SetupByDefault(chunkRegistry);


		return result;
	};

	TArray<FVector2D> GeneratePositionsForChunk(WorldGenerator& worldGenerator, float centerX, float centerY, float chunkSize) const override {

		TArray<FVector2D> result;

		float x0 = centerX - chunkSize / 2;
		float x1 = centerX + chunkSize / 2;
		float y0 = centerY - chunkSize / 2;
		float y1 = centerY + chunkSize / 2;

		float gridSize = 10000;

		int64 seed = worldGenerator.GetSeed() + centerX * 1000 + centerY * 10000;

		for (float x = ceil(x0 / gridSize) * gridSize; x <= floor(x1 / gridSize) * gridSize; x += gridSize) {
			for (float y = ceil(y0 / gridSize) * gridSize; y <= floor(y1 / gridSize) * gridSize; y += gridSize) {
				if (RandomGenerator::BilinearNoiseInterpolation(x, y, seed - 101) > 0.95) {
					result.Add(FVector2D(
						x,
						y
					));
				}
			}
		}
		return result;
	}
};
