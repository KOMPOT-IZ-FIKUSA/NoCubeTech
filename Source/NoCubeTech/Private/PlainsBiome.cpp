// Fill out your copyright notice in the Description page of Project Settings.


#include "PlainsBiome.h"
#include "RandomGenerator.h"

float PlainsBiome::GenerateHeight(float x, float y, int64 seed) {
	/*
	float orientationAngle = RandomGenerator::BilinearNoiseInterpolation(data.x / 1000000, data.y / 1000000, data.seed - 95876) * 3.14;
	float sin_ = sin(orientationAngle);
	float cos_ = cos(orientationAngle);
	float result = 100 * (data.GetHeight32768() * 25 + 100);
	for (int i = 0; i < 10; i++) {
		float generated = RandomGenerator::BilinearNoiseInterpolation((data.x) / 25000 + cos_ * i, (data.y) / 25000 + sin_ * i, data.seed - 95878);
		result += generated * 2000;
	}
	*/
	
	return 0;
}

float PlainsBiome::GetWeight(float x, float y, int64 seed, float bicubic200k) {
	return RandomGenerator::BicubicNoiseInterpolation(x / 200000, y / 200000, seed + 1233);
}