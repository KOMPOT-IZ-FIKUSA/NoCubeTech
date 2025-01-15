// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"

class BiomeGenerator;
class AdditionalObjectsGeneratorRegistry;

// non-serializable
class NOCUBETECH_API WorldGenerator
{
public:
	static const int BiomesCount = 2;

	int64 seed;
	BiomeGenerator* biomeGenerator;
	AdditionalObjectsGeneratorRegistry* additionalObjectsGeneratorRegistry;

	WorldGenerator(int64 seed, BiomeGenerator* biomeGenerator, AdditionalObjectsGeneratorRegistry* additionalObjectsGenerator);

	~WorldGenerator();

	int64 GetSeed() const {
		return seed;
	}

	void SetSeed(int64 seed_);

	float GenerateHeight(float x, float y) const;

	BiomeWeights GenerateBiomeWeightsForAdditionalObjects(float x, float y) const;
};