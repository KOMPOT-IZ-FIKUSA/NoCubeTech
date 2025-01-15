// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractBiome.h"


class NOCUBETECH_API OceanBiome : public AbstractBiome {
public:
	float GenerateHeight(float x, float y, int64 seed);

	static float GetWeight(float x, float y, int64 seed);
};