// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataStructures.h"
#include "GlobalWorldGenerator.h"
#include "ChunkAdditionalObjectsGenerator.generated.h"


/*
* Generation in 2 steps: 
* 1) generate all objects by priority for the chunk
* 2) cross-validate with neighbour chunks
*/
UCLASS()
class NOCUBETECH_API AChunkAdditionalObjectsGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkAdditionalObjectsGenerator();

	UPROPERTY()
	int chunkX;

	UPROPERTY()
	int chunkY;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FActorListGrid objectsRegisteredToCells;

	UPROPERTY()
	TWeakObjectPtr<AGlobalWorldGenerator> worldGenerator;

	UPROPERTY()
	bool additionalObjectsGenerated;

	bool tryFindGeneratorIfNecessary();

	void tryGenerateObjectsIfNecessary();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
