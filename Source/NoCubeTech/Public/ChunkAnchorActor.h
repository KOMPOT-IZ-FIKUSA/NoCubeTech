// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkAdditionalObjectsGenerator.h"
#include "GlobalChunkRegistry.h"
#include "GlobalWorldGenerator.h"
#include "ChunkSave.h"
#include "ChunkSavableActor.h"
#include "BasicChunkData.h"
#include "ChunkAnchorActor.generated.h"


UCLASS()
class NOCUBETECH_API AChunkAnchor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChunkAnchor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	int ticksSinceBeginPlay;

	TWeakObjectPtr<AGlobalChunkRegistry> chunkRegistry;


	USceneComponent* rootSceneComponent;

	AChunkAdditionalObjectsGenerator* objectsGenerator;

	void setSelfLabel();

	void initializeContent();

	/*
	* For each actor in chunk save instance:
	*   create actor if it is not created according to global registry of loaded savable actors
	*/
	void loadActorsWhichAreNotLoaded(UBasicChunkData* chunkDataInstance);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsPositionInBounds(float x, float y);
	bool IsPositionInBounds(FVector pos);

	bool IsGlobalRegistryFound() {
		return chunkRegistry.IsValid();
	}

	AChunkAdditionalObjectsGenerator* GetAdditionalObjectsGenerator();

	float FindDistanceToClosestPlayer();

	void postInitWithChunkRegistry(AGlobalChunkRegistry* chunkRegistry_);

};
