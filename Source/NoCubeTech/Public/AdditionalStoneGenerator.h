// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractAdditionalObjGenerator.h"
#include "AAdditionalObject.h"
#include "Stone.h"
#include "RandomGenerator.h"

/**
 * 
 */
class NOCUBETECH_API AdditionalStoneGenerator : public AbstractAdditionalObjGenerator
{
private:


	static FVector GenerateVector2(float x, float y, float z, float period, int64 seed) {
		return FVector(
			RandomGenerator::TrilinearNoiseInterpolation(x / period, y / period, z / period, (seed++) % 100000),
			RandomGenerator::TrilinearNoiseInterpolation(x / period, y / period, z / period, (seed++) % 100000),
			RandomGenerator::TrilinearNoiseInterpolation(x / period, y / period, z / period, (seed++) % 100000)
		) * 2 - 1;
	}

public:
	AdditionalStoneGenerator();
	virtual ~AdditionalStoneGenerator();

	virtual float GetPriority() const override {
		return 0;
	}

	virtual AAdditionalObject* CreateActor(UWorld* world, AGlobalChunkRegistry* chunkRegistry, WorldGenerator& worldGenerator, float x, float y, UObject* outer) const override {
		FActorSpawnParameters parameters = FActorSpawnParameters();
		
		FVector location = FVector(x, y, worldGenerator.GenerateHeight(x, y));

		AStone* stone = world->SpawnActor<AStone>(location, FRotator::ZeroRotator, parameters);
		GenerateGeometry(stone->vertices, stone->faces, worldGenerator.seed + floor(x) * 1212341 + floor(y) * 8569174);
		stone->SetupByDefault(chunkRegistry);


		return stone;
	};

	static void GenerateGeometry(TArray<FVector>& vertices, TArray<int>& faces, int64 seed) {
		FString msg = TEXT("");
		msg = msg.Appendf(TEXT("Generating stone with seed %d"), (int)(seed % 10000));
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, msg);

		vertices = {};
		faces = {};
		const int horizontalPoints = 3 + 10;
		const int verticalPointsOnHalfIncludingPoles = 2 + 10;

		float horizontalAngle = 2 * PI / horizontalPoints;

		int i = 0;
		for (int yi = 0; yi < verticalPointsOnHalfIncludingPoles; yi++) {
			float pitchAngle = (PI) * (0.5 - (float)yi / (verticalPointsOnHalfIncludingPoles - 1) / 2);
			float sinPitch = sin(pitchAngle);
			float cosPitch = cos(pitchAngle);
			for (int xi = 0; xi < horizontalPoints + 1; xi++) {
				float sin_x = sin(horizontalAngle * xi);
				float cos_x = cos(horizontalAngle * xi);

				FVector& vec = vertices.Add_GetRef({ 100 * cosPitch * cos_x, 100 * cosPitch * sin_x, 100 * sinPitch });

				FVector translation = FVector(0, 0, 0);
				
				translation += GenerateVector2(vec.X, vec.Y, vec.Z, 100, seed) * 31;
				translation += GenerateVector2(vec.X, vec.Y, vec.Z, 66, seed + 1) * 32;
				translation += GenerateVector2(vec.X, vec.Y, vec.Z, 30, seed + 2) * 32;
				vec.X += translation.X;
				vec.Y += translation.Y;

				vec *= 20;

				i = yi * (horizontalPoints + 1) + xi;

				if (yi < verticalPointsOnHalfIncludingPoles - 1 && xi < horizontalPoints) {
					faces.Add(i);
					faces.Add(i + 1);
					faces.Add(i + horizontalPoints + 1);
					faces.Add(i + horizontalPoints + 1);
					faces.Add(i + 1);
					faces.Add(i + horizontalPoints + 2);
				}
			}
		}


	}

	TArray<FVector2D> GeneratePositionsForChunk(WorldGenerator& worldGenerator, float centerX, float centerY, float chunkSize) const override {
		return {FVector2D(centerX, centerY)};
		TArray<FVector2D> result;
		float eightOfSize = chunkSize / 8;

		float x0 = centerX - chunkSize / 2;
		float x1 = centerX + chunkSize / 2;
		float y0 = centerY - chunkSize / 2;
		float y1 = centerY + chunkSize / 2;

		float gridSize = 1000;

		for (float x = ceil(x0 / gridSize) * gridSize; x <= floor(x1 / gridSize) * gridSize; x += gridSize) {
			for (float y = ceil(y0 / gridSize) * gridSize; y <= floor(y1 / gridSize) * gridSize; y += gridSize) {
				result.Add(FVector2D(x, y));
			}
		}
		return result;
	}

};
