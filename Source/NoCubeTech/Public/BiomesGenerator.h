// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"
#include "AbstractBiome.h"

class NOCUBETECH_API BiomeGenerator {
protected:
	int64 seed;
	AbstractBiome* registeredBiomes[BIOMES_COUNT];

public:
	BiomeGenerator(int64 seed);

	int64 GetSeed() {
		return seed;
	}

	void SetSeed(int64 seed_) {
		seed = seed_;
	}

	/*
	* Generates biomes probabilities for height generation by calling each PrioritizedBiomeWeightsModifier procedure and applying a normalizing function.
	*/
	void GenerateBiomesForHeight(float x, float y, BiomeWeights& out) const;

	/*
	* Generates biomes probabilities for additional objects generation by calling each PrioritizedBiomeWeightsModifier procedure and applying a normalizing function.
	*/
	void GenerateBiomesForAdditionalObjects(float x, float y, BiomeWeights& out) const;

	float GenerateWeightedHeight(const BiomeWeights& weights, float x, float y) const;

	AbstractBiome* GetBiomeInstance(int id) {
		return registeredBiomes[id];
	}
};
