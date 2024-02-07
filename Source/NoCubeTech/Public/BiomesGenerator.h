// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"
#include "AbstractBiome.h"

class NOCUBETECH_API BiomeGenerator {
protected:
	int64 seed;
	TArray<AbstractBiome*> registeredBiomes;
	TArray<PrioritizedBiomeWeightsModifier> sortedByPriorityBiomeModifiersForHeight;
	TArray<PrioritizedBiomeWeightsModifier> sortedByPriorityBiomeModifiersForColor;

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
	* Returns probabilities
	* betta: any positive number. The greater betta is the sharper is edge between biomes
	*/
	BiomeWeights GenerateBiomesForHeight(float x, float y, double betta) const;

	/*
	* Generates biomes probabilities for color generation by calling each PrioritizedBiomeWeightsModifier procedure and applying a normalizing function.
	* Returns probabilities
	* betta: any positive number. The greater betta is the sharper is edge between biomes
	*/
	BiomeWeights GenerateBiomesForColor(float x, float y, double betta) const;

	// 1) Checks if instance is already registered and returns if true
	// 2) Adds the biome to registered list
	// 3) Adds its modifiers to modifiers list and sorts it
	void RegisterBiome(AbstractBiome* instance);

	int GetBiomesCount() const {
		return registeredBiomes.Num();
	}

	float GenerateWeightedHeight(const BiomeWeights& weights, BiomeHeightGenerationData& data) const;

	FLinearColor GenerateWeightedColor(const  BiomeColorGenerationData& data, const BiomeWeights& biomeProbabilities) const;

	TArray<float> GenerateAbsoluteDerivatives(float x, float y) const;
};
