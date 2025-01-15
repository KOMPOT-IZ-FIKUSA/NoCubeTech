// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGenerator.h"
#include "BiomesGenerator.h"
#include "RandomGenerator.h"
#include "IncludeBiomes.h"
#include "AdditionalObjectsGeneratorRegistry.h"

WorldGenerator::WorldGenerator(
	int64 seed, BiomeGenerator* biomeGenerator, AdditionalObjectsGeneratorRegistry* additionalObjectsGeneratorRegistry)
	: seed(seed), biomeGenerator(biomeGenerator), additionalObjectsGeneratorRegistry(additionalObjectsGeneratorRegistry)
{
}

WorldGenerator::~WorldGenerator() {
	delete biomeGenerator;
	delete additionalObjectsGeneratorRegistry;
}

void WorldGenerator::SetSeed(int64 seed_) {
	seed = seed_;
	biomeGenerator->SetSeed(seed_);
}


float WorldGenerator::GenerateHeight(float x, float y) const {
	BiomeWeights biomeWeights;
	biomeGenerator->GenerateBiomesForHeight(x, y, biomeWeights);
	float result = biomeGenerator->GenerateWeightedHeight(biomeWeights, x, y);
	return result;
}


BiomeWeights WorldGenerator::GenerateBiomeWeightsForAdditionalObjects(float x, float y) const {
	BiomeWeights biomeWeights;
	biomeGenerator->GenerateBiomesForAdditionalObjects(x, y, biomeWeights);
	return biomeWeights;
}
