// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureGenerationCache.h"


TextureGenerationCache::TextureGenerationCache(const WorldGenerator& generator_) : generator(generator_)
{
}

void TextureGenerationCache::Start(int textureSize_, int interpolationChunkSize_, float initialX_, float initialY_, float deltaPos_) {
	check(textureSize_ % interpolationChunkSize_ == 0);
	check(deltaPos_ > 0);
	check(interpolationChunkSize_ > 0);
	textureSize = textureSize_;
	interpolationChunkSize = interpolationChunkSize_;
	initialX = initialX_;
	initialY = initialY_;
	deltaPos = deltaPos_;
	yIndex = 0;
	chunksCountBySide = textureSize / interpolationChunkSize;
	int c = chunksCountBySide + 1;

	if (dataPoints != nullptr) {
		delete[] dataPoints;
	}
	dataPoints = new BiomeColorGenerationData * [c * c];

	if (biomesPoints != nullptr) {
		delete[] biomesPoints;
	}
	biomesPoints = new BiomeWeights * [(c * c)];

	if (resultBGRA != nullptr) {
		delete[] resultBGRA;
	}
	resultBGRA = new uint8[textureSize * textureSize * 4];

	GenerateInterpolationPoints();

}

TextureGenerationCache::~TextureGenerationCache() {
	delete[] dataPoints;
	delete[] biomesPoints;
	delete[] resultBGRA;
}

void TextureGenerationCache::GenerateInterpolationPoints() {
	float x, y;
	int i = 0;
	float deltaPosChunk = interpolationChunkSize * deltaPos;
	for (int yi = 0; yi < chunksCountBySide + 1; yi++) {
		y = initialY + deltaPosChunk * yi;
		for (int xi = 0; xi < chunksCountBySide + 1; xi++) {
			x = initialX + deltaPosChunk * xi;
			dataPoints[i] = generator.PrepareColorGenerationData(x, y, deltaPos);
			BiomeWeights w = generator.biomeGenerator->GenerateBiomesForColor(x, y, 80);
			biomesPoints[i] = new BiomeWeights(w);
			i++;
		}
	}
}

void TextureGenerationCache::testPrint() {
	FString str1 = TEXT("");
	str1.Appendf(TEXT("chunksCountBySide = %d, textureSize = %d, interpolationChunkSize = %d"), chunksCountBySide, textureSize, interpolationChunkSize);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, str1);
}


void TextureGenerationCache::GenerateRow() {
	if (!IsValid() || IsReady()) {
		return;
	}
	int chunk_yi = yIndex / interpolationChunkSize;
	int chunkInterPointesCountBySide = chunksCountBySide + 1;
	int leftUpChunkIndex = chunk_yi * chunkInterPointesCountBySide;
	float dy = (float)(yIndex % interpolationChunkSize) / (float)interpolationChunkSize;
	BiomeColorGenerationData* floorPointData = BiomeColorGenerationData::Lerp(*dataPoints[leftUpChunkIndex], *dataPoints[leftUpChunkIndex + chunkInterPointesCountBySide], dy);
	BiomeWeights* floorPointBiomes = BiomeWeights::LerpPtr(biomesPoints[leftUpChunkIndex], biomesPoints[leftUpChunkIndex + chunkInterPointesCountBySide], dy);
	BiomeColorGenerationData* ceilPointData = floorPointData;
	BiomeWeights* ceilPointBiomes = floorPointBiomes;

	int i = yIndex * textureSize * 4;

	float dx;
	for (int xi = 0; xi < textureSize; xi++) {
		if (xi % interpolationChunkSize == 0) {
			leftUpChunkIndex += 1;
			if (xi > 0) {
				delete floorPointData;
				delete floorPointBiomes;
			}
			floorPointData = ceilPointData;
			ceilPointData = BiomeColorGenerationData::Lerp(*dataPoints[leftUpChunkIndex], *dataPoints[leftUpChunkIndex + chunkInterPointesCountBySide], dy);
			floorPointBiomes = ceilPointBiomes;
			ceilPointBiomes = BiomeWeights::LerpPtr(biomesPoints[leftUpChunkIndex], biomesPoints[leftUpChunkIndex + chunkInterPointesCountBySide], dy);
		}
		dx = (float)(xi % interpolationChunkSize) / (float)interpolationChunkSize;
		BiomeColorGenerationData* interpolatedForPixelData = BiomeColorGenerationData::Lerp(*floorPointData, *ceilPointData, dx);
		BiomeWeights* interpolatedForPixelBiomes = BiomeWeights::LerpPtr(floorPointBiomes, ceilPointBiomes, dx);
		FLinearColor color = generator.GenerateColor(*interpolatedForPixelData, *interpolatedForPixelBiomes);
		resultBGRA[i] = static_cast<uint8>(color.B * 255);
		resultBGRA[i + 1] = static_cast<uint8>(color.G * 255);
		resultBGRA[i + 2] = static_cast<uint8>(color.R * 255);
		resultBGRA[i + 3] = static_cast<uint8>(color.A * 255);
		delete interpolatedForPixelBiomes;
		delete interpolatedForPixelData;
		i += 4;
	}
	yIndex++;
	delete floorPointData;
	delete floorPointBiomes;
	delete ceilPointData;
	delete ceilPointBiomes;



}

bool TextureGenerationCache::IsValid() {
	return
		textureSize > 0 &&
		yIndex >= 0 &&
		interpolationChunkSize > 0 &&
		textureSize % interpolationChunkSize == 0 &&
		chunksCountBySide > 0;
}

bool TextureGenerationCache::IsReady() {
	return yIndex == textureSize;
}

bool TextureGenerationCache::IsGenerating() {
	return IsValid() && !IsReady();
}

