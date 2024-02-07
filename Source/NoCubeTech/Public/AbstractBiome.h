// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "PrioritizedBiomeWeightsModifier.h"
#include "RandomGenerator.h"
#include "BiomeGenerationData.h"

/*
Represents a biome. One instance of each inherited biome class sholud be registered by BiomeRegistry::RegisterBiome .
Biome id is given when registering instance. id = -1 means instance is not registered.
Modifiers are procedures taking arguments (float x, float y, int seed, BiomeWeights weights) .
Modifiers make an instance able to modify biome map in different phases (different priorities). Bigger priority number means later execution.
*/
class NOCUBETECH_API AbstractBiome {
protected:
	int id = -1;
public:
	virtual float GenerateHeight(BiomeHeightGenerationData& data) = 0;
	virtual FLinearColor GenerateColor(const BiomeColorGenerationData& data) = 0;
	virtual TArray<PrioritizedBiomeWeightsModifier> GetModifiersForHeight() = 0;
	virtual TArray<PrioritizedBiomeWeightsModifier> GetModifiersForColor() = 0;

	int GetId() { return id; }
	void SetRegistered(int idIn) {
		if (id != -1) {
			GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("Trying to register a biome instance that is already registered."));
			UE_LOG(LogTemp, Fatal, TEXT("Trying to register a biome instance that is already registered."));
		}
		id = idIn;
	}
	bool IsRegistered() { return id != -1; }
	
	static inline float sigmoid(float x) {
		return 1 / (1 + exp(fmin(fmax(-15, -x), 15)));
	}
};
