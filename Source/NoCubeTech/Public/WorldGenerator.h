// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesGenerator.h"
#include "GlobalColorGenerationData.h"
#include "AdditionalObjectsGeneratorRegistry.h"

// non-serializable
class NOCUBETECH_API WorldGenerator
{
public:
	int64 seed;
	BiomeGenerator* biomeGenerator;
	GlobalColorGenerationData* globalColorGenerationData;
	AdditionalObjectsGeneratorRegistry* additionalObjectsGeneratorRegistry;

	WorldGenerator(int64 seed, BiomeGenerator* biomeGenerator, GlobalColorGenerationData* globalColorGenerationData, AdditionalObjectsGeneratorRegistry* additionalObjectsGenerator);

	~WorldGenerator();

	int64 GetSeed() const {
		return seed;
	}

	void SetSeed(int64 seed_);

	float GenerateHeight(float x, float y) const;
	FLinearColor GenerateColor(const BiomeColorGenerationData& data, const BiomeWeights& biomeProbabilities) const;

	BiomeHeightGenerationData* PrepareHeightGenerationData(float x, float y) const;
	BiomeColorGenerationData* PrepareColorGenerationData(float x, float y, float unitsDeltaBetweenPixels) const;


protected:
	LandscapeBumpState GenerateBlurredBump(float centerX, float centerY, float areaSize, float centerHeight) const;
};