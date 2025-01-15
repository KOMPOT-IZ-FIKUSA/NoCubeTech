// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"

/*
Holds priority of the BiomeWeights modification procedure
Modification procedure is a static void function(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights)

This class is decided not to be serializable due to the raw function pointer.
*/
class AbstractBiome;
struct NOCUBETECH_API PrioritizedBiomeWeightsModifier {
	uint32 priority;
	void(*modifier)(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights);
	AbstractBiome* biome;

	PrioritizedBiomeWeightsModifier(
		uint32 priority,
		void(*modifier)(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights),
		AbstractBiome* biome)
		:
		priority(priority),
		modifier(modifier),
		biome(biome) {
		
	}

	void Apply(float x, float y, int64 seed, BiomeWeights& weights) {
		modifier(biome, x, y, seed, weights);
	}
};

