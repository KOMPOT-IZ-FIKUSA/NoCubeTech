// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomesGenerator.h"
#include "IncludeBiomes.h"


BiomeGenerator::BiomeGenerator(int64 seed) : seed(seed) {
	registeredBiomes[0] = new PlainsBiome();
	registeredBiomes[1] = new HillsBiome();

	
	// A little validation to check I am not mistaken with indices
	for (int i = 0; i < BIOMES_COUNT; i++) {
		check(registeredBiomes[i]->GetId() == i);
	}

}

void BiomeGenerator::GenerateBiomesForHeight(float x, float y, BiomeWeights& out) const {;

	float bicubic200k = RandomGenerator::BicubicNoiseInterpolation(x / 200000, y / 200000, seed + 1);
	float bicubic200k_1 = RandomGenerator::BicubicNoiseInterpolation(x / 200000, y / 200000, seed + 2);
	out.values[0] = PlainsBiome::GetWeight(x, y, seed, bicubic200k);
	out.values[1] = HillsBiome::GetWeight(x, y, seed, bicubic200k_1);

	out.ApplyNormalization(20);
}

void BiomeGenerator::GenerateBiomesForAdditionalObjects(float x, float y, BiomeWeights& out) const {
	GenerateBiomesForHeight(x, y, out);
}


float BiomeGenerator::GenerateWeightedHeight(const BiomeWeights& weights, float x, float y) const {
	float result = 0;
	const float eps = 0.01;
	for (int i = 0; i < BIOMES_COUNT; i++) {
		if (weights.values[i] > eps) {
			result += registeredBiomes[i]->GenerateHeight(x, y, seed) * weights.values[i];
		}
	}
	return result;
}