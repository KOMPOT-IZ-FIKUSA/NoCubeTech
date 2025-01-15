// Fill out your copyright notice in the Description page of Project Settings.



#include "OceanBiome.h"
#include "RandomGenerator.h"

float OceanBiome::GenerateHeight(float x, float y, int64 seed) {
	return 100 * (-20);
}

float OceanBiome::GetWeight(float x, float y, int64 seed) {
	return RandomGenerator::BicubicNoiseInterpolation(x / 1500000, y / 1500000, seed + 3554643645465);
}