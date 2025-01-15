// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractBiome.h"


class NOCUBETECH_API PlainsBiome : public AbstractBiome {
public:
	static const int PlainsBiomeId = 0;

	float GenerateHeight(float x, float y, int64 seed);

	static float GetWeight(float x, float y, int64 seed, float bicubic200k);

	PlainsBiome() : AbstractBiome(PlainsBiomeId) {

	}

	
};