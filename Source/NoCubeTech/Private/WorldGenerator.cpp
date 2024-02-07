// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "BiomesGenerator.h"
#include "RandomGenerator.h"
#include "BiomeGenerationData.h"

WorldGenerator::WorldGenerator(
	int64 seed, BiomeGenerator* biomeGenerator, GlobalColorGenerationData* globalColorGenerationData, AdditionalObjectsGeneratorRegistry* additionalObjectsGeneratorRegistry)
	: seed(seed), biomeGenerator(biomeGenerator), globalColorGenerationData(globalColorGenerationData), additionalObjectsGeneratorRegistry(additionalObjectsGeneratorRegistry)
{
}

WorldGenerator::~WorldGenerator() {
	delete biomeGenerator;
	delete globalColorGenerationData;
	delete additionalObjectsGeneratorRegistry;
}

void WorldGenerator::SetSeed(int64 seed_) {
	seed = seed_;
	biomeGenerator->SetSeed(seed_);
}


float WorldGenerator::GenerateHeight(float x, float y) const {
	BiomeWeights biomeWeights = biomeGenerator->GenerateBiomesForHeight(x, y, 20);
	BiomeHeightGenerationData* data = PrepareHeightGenerationData(x, y);
	float result = biomeGenerator->GenerateWeightedHeight(biomeWeights, *data);
	delete data;
	return result;
}

FLinearColor WorldGenerator::GenerateColor(const BiomeColorGenerationData& data, const BiomeWeights& biomeProbabilities) const {
	return biomeGenerator->GenerateWeightedColor(data, biomeProbabilities);
}

BiomeHeightGenerationData* WorldGenerator::PrepareHeightGenerationData(float x, float y) const {
	return new BiomeHeightGenerationData(x, y, seed);
}

BiomeColorGenerationData* WorldGenerator::PrepareColorGenerationData(float x, float y, float unitsDeltaBetweenPixels) const {
	float calculatedHeight = GenerateHeight(x, y);
	return new BiomeColorGenerationData(x, y, seed,
		calculatedHeight,
		GenerateHeight(x + 1, y) - calculatedHeight,
		GenerateHeight(x, y + 1) - calculatedHeight,
		*globalColorGenerationData,
		unitsDeltaBetweenPixels,
		GenerateBlurredBump(x, y, 100000, calculatedHeight )
		);
}

LandscapeBumpState WorldGenerator::GenerateBlurredBump(float centerX, float centerY, float areaSize, float centerHeight) const {
	areaSize /= 2;

	float sumDeltaAbs = 0;


	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX + areaSize, centerY));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX + areaSize, centerY + areaSize));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX + areaSize, centerY - areaSize));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX, centerY - areaSize));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX, centerY + areaSize));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX - areaSize, centerY + areaSize));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX - areaSize, centerY - areaSize));
	sumDeltaAbs += abs(centerHeight - GenerateHeight(centerX - areaSize, centerY));
	
	return LandscapeBumpState(
		sumDeltaAbs / 8
	);
}