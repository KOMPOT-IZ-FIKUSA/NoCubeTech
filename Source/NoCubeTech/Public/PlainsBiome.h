// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractBiome.h"
#include "BiomeGenerationData.h"


class NOCUBETECH_API PlainsBiome : public AbstractBiome {
public:
	float GenerateHeight(BiomeHeightGenerationData& data);
	FLinearColor GenerateColor(const BiomeColorGenerationData& data);
	TArray<PrioritizedBiomeWeightsModifier> GetModifiersForHeight();
	TArray<PrioritizedBiomeWeightsModifier> GetModifiersForColor();

	static void initial_value_modifier(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights);
};