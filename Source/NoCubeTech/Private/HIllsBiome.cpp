// Fill out your copyright notice in the Description page of Project Settings.


#include "HIllsBiome.h"
#include "BiomeGenerationData.h"
#include "RandomGenerator.h"



FLinearColor HillsBiome::GenerateColor(const BiomeColorGenerationData& data) {

	const float grassFrequency = 1;
	const float grassScale = data.delta / grassFrequency;
	const FLinearColor colorGrass1 = data.globalColorGenerationData.grassTexture->GetColorNearest(data.x * grassFrequency, data.y * grassFrequency, grassScale);
	FLinearColor colorGrass = colorGrass1;

	const float rockFrequency = 1;
	const float rockScale = data.delta / rockFrequency;

	const float dirtFrequency = 1;
	const float dirtScale = data.delta / dirtFrequency;


	float displacementX = RandomGenerator::BilinearNoiseInterpolation(data.x / 1000, data.y / 1000, data.seed + 2845269151145);
	float displacementY = RandomGenerator::BilinearNoiseInterpolation(data.x / 1000, data.y / 1000, data.seed + 2845269151146) * (1 - displacementX);
	const FLinearColor colorRock1 = data.globalColorGenerationData.rockTexture->GetColorNearest((data.x + displacementX * 1000) * rockFrequency, (data.y + displacementY * 1000) * rockFrequency, rockScale);

	displacementX = RandomGenerator::BilinearNoiseInterpolation(data.x / 1000, data.y / 1000, data.seed + 2845269151142345);
	displacementY = RandomGenerator::BilinearNoiseInterpolation(data.x / 1000, data.y / 1000, data.seed + 2845269151123446) * (1 - displacementX);
	const FLinearColor colorRock2 = data.globalColorGenerationData.rockTexture2->GetColorNearest((data.x + displacementX * 1000) * rockFrequency, (data.y + displacementY * 1000) * rockFrequency, rockScale);

	displacementX = RandomGenerator::BilinearNoiseInterpolation(data.x / 4000, data.y / 4000, data.seed + 2845269151452345);
	displacementY = RandomGenerator::BilinearNoiseInterpolation(data.x / 4000, data.y / 4000, data.seed + 28452651123446) * (1 - displacementX);
	FLinearColor colorRock3 = data.globalColorGenerationData.rockTexture3->GetColorNearest((data.x + displacementX * 4000) * rockFrequency / 2, (data.y + displacementY * 4000) * rockFrequency / 2, rockScale * 2);
	float randomBrightnessStone = RandomGenerator::BicubicNoiseInterpolation(data.x / 4000, data.y / 4000, data.seed + 120962545617);
	randomBrightnessStone = 0.5 + randomBrightnessStone * 0.5;
	colorRock3 *= randomBrightnessStone;

	float steepness = data.bump1000Meters.averageDeltaAbs;

	float rockInterpolationWeight1 = RandomGenerator::BilinearNoiseInterpolation(data.x / 8000, data.y / 8000, data.seed + 378891322432354) + 0.3;
	float rockInterpolationWeight2 = RandomGenerator::BilinearNoiseInterpolation(data.x / 800, data.y / 800, data.seed + 37889132245320) + 0.1;
	float rockInterpolationWeight3 = RandomGenerator::BilinearNoiseInterpolation(data.x / 600, data.y / 600, data.seed + 378891322420) / 10 + 0.01 + steepness / 150000 + (sin(data.calculatedHeight / 15000) + 1) / 8 + data.calculatedHeight / 2500000 + (sin(data.x / 50000) + 1) / 10;
	rockInterpolationWeight1 = rockInterpolationWeight1 * rockInterpolationWeight1;
	rockInterpolationWeight2 = rockInterpolationWeight2 * rockInterpolationWeight2;
	rockInterpolationWeight3 = rockInterpolationWeight3 * rockInterpolationWeight3 * rockInterpolationWeight3 * rockInterpolationWeight3;
	FLinearColor colorRock =
		(
			colorRock1 * rockInterpolationWeight1 +
			colorRock2 * rockInterpolationWeight2 +
			colorRock3 * rockInterpolationWeight3
		) / (
			rockInterpolationWeight1 +
			rockInterpolationWeight2 +
			rockInterpolationWeight3
			);


	FLinearColor colorDirt = data.globalColorGenerationData.dirtTexture1->GetColorNearest(data.x * dirtFrequency, data.y * dirtFrequency, dirtScale);

	float soilWeight = 0;

	//soilWeight += RandomGenerator::BicubicNoiseInterpolation(data.x / 2000, data.y / 2000, data.seed + 31289873455346564) / 10;


	soilWeight += (10000 - steepness) / 500;
	soilWeight = sigmoid(soilWeight);

	float grassWeight = sigmoid(soilWeight * 20 - 15);
	float randomBrightness = 0;
	randomBrightness += RandomGenerator::BicubicNoiseInterpolation(data.x / 4000, data.y / 4000, data.seed + 120962545617);
	randomBrightness += RandomGenerator::BicubicNoiseInterpolation(data.x / 2000, data.y / 2000, data.seed + 12096545617);
	randomBrightness += RandomGenerator::BicubicNoiseInterpolation(data.x / 1000, data.y / 1000, data.seed + 12096545615);
	randomBrightness += RandomGenerator::BicubicNoiseInterpolation(data.x / 500, data.y / 500, data.seed + 1209645615);
	FLinearColor result = soilWeight * ((colorDirt * (1 - grassWeight) + colorGrass * grassWeight)) + (1 - soilWeight) * colorRock;
	result = result * (randomBrightness / 4 * 0.25 + 0.75);
	return result;
}
void HillsBiome::initialValueModifierForHeight(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights) {
	weights.values[biomeInstance->GetId()] = RandomGenerator::BicubicNoiseInterpolation(x / 400000, y / 400000, seed - 80621);
}
void HillsBiome::initialValueModifierForColor(AbstractBiome* biomeInstance, float x, float y, int64 seed, BiomeWeights& weights) {
	weights.values[biomeInstance->GetId()] = RandomGenerator::BicubicNoiseInterpolation(x / 400000, y / 400000, seed - 80621) + 0.15;
}

TArray<PrioritizedBiomeWeightsModifier> HillsBiome::GetModifiersForHeight() {
	TArray<PrioritizedBiomeWeightsModifier> result;
	result.Add(PrioritizedBiomeWeightsModifier(0, &initialValueModifierForHeight, this));
	return result;
}

TArray<PrioritizedBiomeWeightsModifier> HillsBiome::GetModifiersForColor() {
	TArray<PrioritizedBiomeWeightsModifier> result;
	result.Add(PrioritizedBiomeWeightsModifier(0, &initialValueModifierForColor, this));
	return result;
}

float HillsBiome::GenerateHeight(BiomeHeightGenerationData& data) {
	float orientationAngle = RandomGenerator::BilinearNoiseInterpolation(data.x / 1000000, data.y / 1000000, data.seed - 95876) * 3.14;

	float sin_ = sin(orientationAngle);
	float cos_ = cos(orientationAngle);

	float result = 0;
	float conv = 0;
	for (int i = 0; i < 8; i++) {
		float generated = RandomGenerator::BilinearNoiseInterpolation((data.x) / 6000 + cos_ * i, (data.y) / 6000 + sin_ * i, data.seed - 95877);
		result += generated * 2000;
	}

	conv = 0;
	for (int i = 0; i < 10; i++) {
		float generated = RandomGenerator::BilinearNoiseInterpolation((data.x) / 12000 + cos_ * i, (data.y) / 12000 + sin_ * i, data.seed - 958771);
		result += generated * 3000;
	}

	conv = 0;
	for (int i = 0; i < 10; i++) {
		float generated = RandomGenerator::BilinearNoiseInterpolation((data.x) / 25000 + cos_ * i, (data.y) / 25000 + sin_ * i, data.seed - 95878);
		result += generated * 4000;
	}

	conv = 0;
	float generated = RandomGenerator::BilinearNoiseInterpolation((data.x) / 200000, (data.y) / 200000, data.seed - 95879);
	conv += generated;
	result += conv * conv * conv * 350000;
	result *= 0.9;
	return result;
}