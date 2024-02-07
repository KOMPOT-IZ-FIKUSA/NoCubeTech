// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalWorldGenerator.h"
#include "HillsBiome.h"
#include "AdditionalStoneGenerator.h"
#include "PlainsBiome.h"

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
	GlobalColorGenerationData* globalColorGenerationData = new GlobalColorGenerationData();
	if (grassTexture == nullptr) {
		// TODO: handle exceptions
	}
	if (rockTexture == nullptr) {

	}
	globalColorGenerationData->grassTexture = new TextureReadonlySource(grassTexture);

	globalColorGenerationData->rockTexture = new TextureReadonlySource(rockTexture);
	globalColorGenerationData->rockTexture2 = new TextureReadonlySource(rockTexture2);
	globalColorGenerationData->rockTexture3 = new TextureReadonlySource(rockTexture3);

	globalColorGenerationData->dirtTexture1 = new TextureReadonlySource(dirtTexture1);

	int64 seed = 2;


	HillsBiome* hillsBiome = new HillsBiome();
	PlainsBiome* plainsBiome = new PlainsBiome();

	BiomeGenerator* biomeGenerator = new BiomeGenerator(seed);
	biomeGenerator->RegisterBiome(hillsBiome);
	biomeGenerator->RegisterBiome(plainsBiome);

	AdditionalObjectsGeneratorRegistry* additionalObjectsGeneratorRegistry = new AdditionalObjectsGeneratorRegistry();
	AdditionalStoneGenerator* stoneGenerator = new AdditionalStoneGenerator();
	additionalObjectsGeneratorRegistry->RegisterGeneratorForBiome(hillsBiome, stoneGenerator);

	generator = new WorldGenerator(seed, biomeGenerator, globalColorGenerationData, additionalObjectsGeneratorRegistry);
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

