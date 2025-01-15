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

	static int IntToInt(int a);

	static float IntToFloat(int a);

	static float Int_x_y_seed_ToFloat(int x, int y, int seed);

	static float Int_x_y_z_seed_ToFloat(int x, int y, int z, int seed);

	static float BilinearNoiseInterpolation(float x, float y, int seed);

	static float BicubicNoiseInterpolation(float x, float y, int seed);

	
	static float PeriodicBilinearNoiseInterpolation(float x, float y, int periodX, int periodY, int seed);

	static float TrilinearNoiseInterpolation(float x, float y, float z, int seed);
};
