// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobalChunkRegistry.h"
#include "GlobalWorldGenerator.h"
#include "ChunkSave.h"
#include "ChunkSavableActor.h"
#include "ChunkAnchorActor.generated.h"


UCLASS()
class NOCUBETECH_API AChunkAnchor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChunkAnchor();

	/*
	* Removes actor from map of prepared for saving if it is there
	*/
	void RemoveActorFromPreparedToSave(int actorId);
	
	/*
	* Adds or sets data of saved actor in the map of prepared actors
	*/
	void SetActorPreparedToSave(int actorId, FSavedActorContainer container);

	static FString GetSaveSlotName(float worldPartitionCellSize, float x, float y) {

	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool TryFindChunkRegistry();
	bool CreateNeighbourChunksIfNeccesarily();
	bool CreateChunkIfNeccesary(float worldX, float worldY);

	TWeakObjectPtr<AGlobalChunkRegistry> chunkRegistry;

	USceneComponent* rootSceneComponent;

	bool createdOrRestored;

	/*
	* actor uobject uid -> saved container
	*/
	TMap<int, FSavedActorContainer> actorsPreparedToSave;

	void saveAllData();

	/*
	* Find actors that are on this chunk
	*/
	void prepareAllActorsForSaving();

	void initializeContent();

	void setSelfLabel();

	void loadContentOrInitIfNeccesarily();

	FString getSaveSlotName();

	/*
	* Called when chunkRegistry is found.
	* Creates content if the chunk was just created.
	* Shows an error if chunk was already loaded (according to chunkRegistry data)
	*/
	void handleChunkRegistryFound();


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsPositionInBounds(float x, float y);
	bool IsPositionInBounds(FVector pos);

	bool IsGlobalRegistryFound() {
		return chunkRegistry.IsValid();
	}

};
