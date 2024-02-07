// Fill out your copyright notice in the Description page of Project Settings.


#include "PlainsBiome.h"
#include "BiomeGenerationData.h"
#include "RandomGenerator.h"

float PlainsBiome::GenerateHeight(BiomeHeightGenerationData& data) {
	return 100 * (data.GetAverageHeight1() * 100 + data.GetAverageHeight2() * 10);
}
FLinearColor PlainsBiome::GenerateColor(const BiomeColorGenerationData& data) {
	return FLinearColor(0, 0.6, 0.05);
}

void PlainsBiome::initial_value_modifier(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights) {
	weights.values[biomeInstance->GetId()] = RandomGenerator::BicubicNoiseInterpolation(x / 400000, y / 400000, seed + 1234567545389);
}

TArray<PrioritizedBiomeWeightsModifier> PlainsBiome::GetModifiersForHeight() {
	TArray<PrioritizedBiomeWeightsModifier> result;
	result.Add(PrioritizedBiomeWeightsModifier(0, &initial_value_modifier, this));
	return result;
}
TArray<PrioritizedBiomeWeightsModifier> PlainsBiome::GetModifiersForColor() {
	TArray<PrioritizedBiomeWeightsModifier> result;
	result.Add(PrioritizedBiomeWeightsModifier(0, &initial_value_modifier, this));
	return result;
}