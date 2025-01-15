// Fill out your copyright notice in the Description page of Project Settings.


#include "AdditionalObjectsGeneratorRegistry.h"

AdditionalObjectsGeneratorRegistry::AdditionalObjectsGeneratorRegistry()
{
	registeredGeneratorsSortedByPriority = TArray<FAdditionalGeneratorWithBiome>();
	registeredGenerators = TArray<AbstractAdditionalObjGenerator*>();
}

AdditionalObjectsGeneratorRegistry::~AdditionalObjectsGeneratorRegistry()
{
}


void AdditionalObjectsGeneratorRegistry::RegisterGeneratorForBiome(AbstractBiome* biome, AbstractAdditionalObjGenerator* generator) {
	if (!biome || !generator) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Yellow, TEXT("Cannot register biome generator: nullptr"));
		UE_LOG(LogTemp, Error, TEXT("Cannot register biome generator: nullptr"));
	}

	if (generator->IsRegistered()) {
		check(generator->GetId() < registeredGenerators.Num());
	}
	else {
		generator->SetRegistered(registeredGenerators.Num());
		registeredGenerators.Add(generator);
	}

	// Check if the combination already exists
	for (const FAdditionalGeneratorWithBiome& registeredPair : registeredGeneratorsSortedByPriority) {
		if (registeredPair.biome == biome && registeredPair.generator == generator) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Yellow, TEXT("Generator is already registered for this Biome"));
			UE_LOG(LogTemp, Warning, TEXT("Generator is already registered for this Biome"));
			return;
		}
	}


	registeredGeneratorsSortedByPriority.Add(FAdditionalGeneratorWithBiome(generator, biome));
	registeredGeneratorsSortedByPriority.Sort([]
	(const FAdditionalGeneratorWithBiome& a, const FAdditionalGeneratorWithBiome& b) {
			return a.generator->GetPriority() < b.generator->GetPriority();
		});
}

TArray<FAdditionalGeneratorWithBiome> AdditionalObjectsGeneratorRegistry::GetGeneratorsSortedByPriority(TSet<int> biomes) {
	TArray<FAdditionalGeneratorWithBiome> result = TArray<FAdditionalGeneratorWithBiome>();
	for (const FAdditionalGeneratorWithBiome& pair : registeredGeneratorsSortedByPriority) {
		int biomeIndex = pair.biome->GetId();
		if (biomes.Contains(biomeIndex)) {
			result.Add(pair);
		}
	}
	return result;
}