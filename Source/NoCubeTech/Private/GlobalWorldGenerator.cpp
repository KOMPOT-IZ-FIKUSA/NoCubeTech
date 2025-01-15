// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalWorldGenerator.h"
#include "HillsBiome.h"
#include "AdditionalStoneGenerator.h"
#include "IncludeBiomes.h"
#include "FlatTerrainPartGenerator.h"
#include "BiomesGenerator.h"

// Sets default values
AGlobalWorldGenerator::AGlobalWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	generator = nullptr;

}



// Called when the game starts or when spawned
void AGlobalWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (!generator) {
		initGenerator();
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, TEXT("Initialized generator"));
	}
	
}

void AGlobalWorldGenerator::initGenerator() {

	int64 seed = 1;

	BiomeGenerator* biomeGenerator = new BiomeGenerator(seed);

	AdditionalObjectsGeneratorRegistry* additionalObjectsGeneratorRegistry = new AdditionalObjectsGeneratorRegistry();
	AdditionalStoneGenerator* stoneGenerator = new AdditionalStoneGenerator();
	FlatTerrainPartGenerator* flatTerrainPartGenerator = new FlatTerrainPartGenerator();

	
	additionalObjectsGeneratorRegistry->RegisterGeneratorForBiome(biomeGenerator->GetBiomeInstance(HillsBiome::HillsBiomeId), flatTerrainPartGenerator);
	
	generator = new WorldGenerator(seed, biomeGenerator, additionalObjectsGeneratorRegistry);
}



void AGlobalWorldGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (generator) {
		delete generator;
	}
}

// Called every frame
void AGlobalWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

