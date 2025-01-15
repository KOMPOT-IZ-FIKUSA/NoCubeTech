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
class NOCUBETECH_API AdditionalObjectsGeneratorRegistry
{
protected:
	TArray<AbstractAdditionalObjGenerator*> registeredGenerators;
	TArray<FAdditionalGeneratorWithBiome> registeredGeneratorsSortedByPriority;
	

public:

	AdditionalObjectsGeneratorRegistry();
	~AdditionalObjectsGeneratorRegistry();

	void RegisterGeneratorForBiome(AbstractBiome* biome, AbstractAdditionalObjGenerator* generator);

	AbstractAdditionalObjGenerator* GetGeneratorById(int id) {
		check(id >= 0);
		check(id < registeredGenerators.Num());
		return registeredGenerators[id];
	}

	TArray<FAdditionalGeneratorWithBiome> GetGeneratorsSortedByPriority(TSet<int> biomes);
};
