// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractBiome.h"

struct BiomeWeights;

class NOCUBETECH_API HillsBiome : public AbstractBiome {
public:
	static const int HillsBiomeId = 1;

	float GenerateHeight(float x, float y, int64 seed);

	static void initialValueModifierForHeight(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights);
	static void initialValueModifierForColor(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights);

	HillsBiome() : AbstractBiome(HillsBiomeId) {

	}

	static float GetWeight(float x, float y, int64 seed, float bicubic200k) {
		return RandomGenerator::BicubicNoiseInterpolation(x / 200000, y / 200000, seed - 80621);
	}

private:
	static float FikusGrid(float x, float y) {
		x = x - floor(x) - 0.5;
		y = y - floor(y) - 0.5;
		return 4 * fmax(x * x, y * y);
	}

};