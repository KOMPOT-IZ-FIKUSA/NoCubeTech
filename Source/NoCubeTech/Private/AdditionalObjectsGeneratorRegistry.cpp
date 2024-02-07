// Fill out your copyright notice in the Description page of Project Settings.


#include "AdditionalObjectsGeneratorRegistry.h"

AdditionalObjectsGeneratorRegistry::AdditionalObjectsGeneratorRegistry()
{
	registeredGenerators = TArray<FAdditionalGeneratorWithBiome>();
}

AdditionalObjectsGeneratorRegistry::~AdditionalObjectsGeneratorRegistry()
{
}


void AdditionalObjectsGeneratorRegistry::RegisterGeneratorForBiome(AbstractBiome* biome, AbstractAdditionalObjGenerator* generator) {
	if (!biome || !generator) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Yellow, TEXT("Cannot register biome generator: nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Cannot register biome generator: nullptr"));
	}

	// Check if the combination already exists
	for (const FAdditionalGeneratorWithBiome& registeredPair : registeredGenerators) {
		if (registeredPair.biome == biome && registeredPair.generator == generator) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Yellow, TEXT("Generator is already registered for this Biome"));
			UE_LOG(LogTemp, Warning, TEXT("Generator is already registered for this Biome"));
			return;
		}
	}

	registeredGenerators.Add(FAdditionalGeneratorWithBiome(generator, biome));
	registeredGenerators.Sort([]
	(const FAdditionalGeneratorWithBiome& a, const FAdditionalGeneratorWithBiome& b) {
			return a.generator->GetPriority() < b.generator->GetPriority();
		});
}

TArray<FPrioritizedBiomeToGeneratorsMap> AdditionalObjectsGeneratorRegistry::GetGeneratorsSortedByPriority(TSet<int> biomes) {
	TMap<float, FPrioritizedBiomeToGeneratorsMap> result1 = TMap<float, FPrioritizedBiomeToGeneratorsMap>();
	for (const FAdditionalGeneratorWithBiome& pair : registeredGenerators) {
		int biomeIndex = pair.biome->GetId();
		if (biomes.Contains(biomeIndex)) {
			float priority = pair.generator->GetPriority();
			if (!result1.Contains(priority)) {
				result1[priority] = FPrioritizedBiomeToGeneratorsMap(priority);
			}
			if (!result1[priority].biomesToGenerators.Contains(biomeIndex)) {
				result1[priority].biomesToGenerators.Add(biomeIndex, FGeneratorArray());
			}
			result1[priority].biomesToGenerators[biomeIndex].generators.Add(pair.generator);
		}
	}

	TArray<FPrioritizedBiomeToGeneratorsMap> result2 = TArray<FPrioritizedBiomeToGeneratorsMap>();
	result1.GenerateValueArray(result2);
	result2.Sort(
		[](const FPrioritizedBiomeToGeneratorsMap& a, const FPrioritizedBiomeToGeneratorsMap& b) {
			return a.priority < b.priority;
		}
	);
	return result2;
}