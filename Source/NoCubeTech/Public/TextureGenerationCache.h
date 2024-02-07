// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"
#include "WorldGenerator.h"


/**
 * non-serializable
 */
class NOCUBETECH_API TextureGenerationCache
{
public:
	TextureGenerationCache(const WorldGenerator& generator_);
	~TextureGenerationCache();

	void Start(int textureSize, int interpolationChunkSize, float initialX, float initialY, float deltaPos);
	bool IsValid();
	bool IsReady();
	bool IsGenerating();
	void GenerateRow();

	uint8* resultBGRA = nullptr;

protected:
	int yIndex = -1;
	int textureSize = -1;
	int interpolationChunkSize = -1;
	float initialX = 0;
	float initialY = 0;
	float deltaPos = 0;
	int chunksCountBySide = -1;
	BiomeColorGenerationData** dataPoints = nullptr;
	BiomeWeights** biomesPoints = nullptr;
	const WorldGenerator& generator;
	void GenerateInterpolationPoints();

	void testPrint();

};
