// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractBiome.h"
#include "AbstractAdditionalObjGenerator.h"

// non-serializable
struct NOCUBETECH_API FAdditionalGeneratorWithBiome {
public:
	AbstractAdditionalObjGenerator* generator;
	AbstractBiome* biome;


	FAdditionalGeneratorWithBiome(AbstractAdditionalObjGenerator* generator, AbstractBiome* biome)
		: generator(generator), biome(biome)
	{
	}
};

// non-serializable
struct NOCUBETECH_API FGeneratorArray {
public:
	TArray<AbstractAdditionalObjGenerator*> generators;

	FGeneratorArray(const TArray<AbstractAdditionalObjGenerator*>& generators)
		: generators(generators)
	{
	}

	FGeneratorArray() {
		generators = TArray<AbstractAdditionalObjGenerator*>();
	}
};

/*
* non-serializable
* Fields:
* float priority;
* TMap<int, FGeneratorArray> generators; - mapping between biome id and list of generators
*/
struct NOCUBETECH_API FPrioritizedBiomeToGeneratorsMap {
public:
	float priority;
	TMap<int, FGeneratorArray> biomesToGenerators;

	FPrioritizedBiomeToGeneratorsMap(float priority) {
		biomesToGenerators = TMap<int, FGeneratorArray>();
	}
};

// non-serializable
class NOCUBETECH_API AdditionalObjectsGeneratorRegistry
{
protected:
	TArray<FAdditionalGeneratorWithBiome> registeredGenerators;

public:
	AdditionalObjectsGeneratorRegistry();
	~AdditionalObjectsGeneratorRegistry();

	void RegisterGeneratorForBiome(AbstractBiome* biome, AbstractAdditionalObjGenerator* generator);

	TArray<FPrioritizedBiomeToGeneratorsMap> GetGeneratorsSortedByPriority(TSet<int> biomes);
};
