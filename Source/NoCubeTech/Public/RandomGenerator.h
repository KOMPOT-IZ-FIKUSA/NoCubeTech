// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NOCUBETECH_API RandomGenerator
{
public:
	RandomGenerator();
	~RandomGenerator();

	static int64 IntToInt(int64 a);

	static float IntToFloat(int64 a);

	static float Int_x_y_seed_ToFloat(int64 x, int64 y, int64 seed);

	static float BilinearNoiseInterpolation(float x, float y, int64 seed);

	static float BicubicNoiseInterpolation(float x, float y, int64 seed);
};
