// Fill out your copyright notice in the Description page of Project Settings.



#include "OceanBiome.h"
#include "BiomeGenerationData.h"
#include "RandomGenerator.h"

float OceanBiome::GenerateHeight(BiomeHeightGenerationData& data) {
	return 100 * (-20);
}
FLinearColor OceanBiome::GenerateColor(const BiomeColorGenerationData& data) {
	return FLinearColor::Blue;
}

void OceanBiome::initial_value_modifier(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights) {
	weights.values[biomeInstance->GetId()] = RandomGenerator::BicubicNoiseInterpolation(x / 1500000, y / 1500000, seed + 3554643645465);
}

TArray<PrioritizedBiomeWeightsModifier> OceanBiome::GetModifiersForHeight() {
	TArray<PrioritizedBiomeWeightsModifier> result;
	result.Add(PrioritizedBiomeWeightsModifier(0, &initial_value_modifier, this));
	return result;
}

TArray<PrioritizedBiomeWeightsModifier> OceanBiome::GetModifiersForColor() {
	TArray<PrioritizedBiomeWeightsModifier> result;
	result.Add(PrioritizedBiomeWeightsModifier(0, &initial_value_modifier, this));
	return result;
}