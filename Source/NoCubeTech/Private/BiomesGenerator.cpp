// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomesGenerator.h"
#include "PrioritizedBiomeWeightsModifier.h"
#include "AbstractBiome.h"


BiomeGenerator::BiomeGenerator(int64 seed) : seed(seed) {
	sortedByPriorityBiomeModifiersForHeight = TArray<PrioritizedBiomeWeightsModifier>();
	sortedByPriorityBiomeModifiersForColor = TArray<PrioritizedBiomeWeightsModifier>();
	registeredBiomes = TArray<AbstractBiome*>();
}

BiomeWeights BiomeGenerator::GenerateBiomesForHeight(float x, float y, double betta) const {
	BiomeWeights result(registeredBiomes.Num(), 0);
	for (PrioritizedBiomeWeightsModifier modifier : sortedByPriorityBiomeModifiersForHeight) {
		modifier.Apply(x, y, seed, result);
	}
	result.ApplyNormalization(betta);
	return result;
}

BiomeWeights BiomeGenerator::GenerateBiomesForColor(float x, float y, double betta) const {
	BiomeWeights result(registeredBiomes.Num(), 0);
	for (PrioritizedBiomeWeightsModifier modifier : sortedByPriorityBiomeModifiersForColor) {
		modifier.Apply(x, y, seed, result);
	}
	result.ApplyNormalization(betta);
	return result;
}

void BiomeGenerator::RegisterBiome(AbstractBiome* instance) {
	check(instance != nullptr)
		for (AbstractBiome* registeredBiome : registeredBiomes) {
			if (registeredBiome == instance) {
				return;
			}
		}
	// add an instance
	registeredBiomes.Add(instance);
	instance->SetRegistered(registeredBiomes.Num() - 1);
	// add weights for height modifiers
	for (PrioritizedBiomeWeightsModifier modifier : instance->GetModifiersForHeight()) {
		sortedByPriorityBiomeModifiersForHeight.Add(modifier);
	}
	sortedByPriorityBiomeModifiersForHeight.Sort([](const PrioritizedBiomeWeightsModifier& a, const PrioritizedBiomeWeightsModifier& b) { return a.priority < b.priority; });
	// add weights for color modifiers
	for (PrioritizedBiomeWeightsModifier modifier : instance->GetModifiersForColor()) {
		sortedByPriorityBiomeModifiersForColor.Add(modifier);
	}
	sortedByPriorityBiomeModifiersForColor.Sort([](const PrioritizedBiomeWeightsModifier& a, const PrioritizedBiomeWeightsModifier& b) { return a.priority < b.priority; });
}

float BiomeGenerator::GenerateWeightedHeight(const BiomeWeights& weights, BiomeHeightGenerationData& data) const {
	float result = 0;
	const float eps = 0.01;
	for (AbstractBiome* biome : registeredBiomes) {
		if (weights.values[biome->GetId()] > eps) {
			result += biome->GenerateHeight(data) * weights.values[biome->GetId()];
		}
	}
	return result;
}

FLinearColor BiomeGenerator::GenerateWeightedColor(const BiomeColorGenerationData& data, const BiomeWeights& biomeProbabilities) const {
	FLinearColor result = FLinearColor::Black;
	const float eps = 0.01;
	for (AbstractBiome* biome : registeredBiomes) {
		if (biomeProbabilities.values[biome->GetId()] > eps) {
			result += biome->GenerateColor(data) * biomeProbabilities.values[biome->GetId()];
		}
	}
	return result;
}

TArray<float> BiomeGenerator::GenerateAbsoluteDerivatives(float x, float y) const {
	BiomeHeightGenerationData point(x, y, seed);
	const float d = 1;
	BiomeHeightGenerationData pointBiasedX(x + d, y, seed);
	BiomeHeightGenerationData pointBiasedY(x, y + d, seed);
	TArray<float> biomeAbsoluteDerivatives;
	for (int i = 0; i < GetBiomesCount(); i++) {
		AbstractBiome* biome = registeredBiomes[i];
		float pointHeight = biome->GenerateHeight(point);
		biomeAbsoluteDerivatives.Add(
			(
				abs(biome->GenerateHeight(pointBiasedX) - pointHeight) +
				abs(biome->GenerateHeight(pointBiasedY) - pointHeight)
				) / d
		);
	}
	return biomeAbsoluteDerivatives;
}