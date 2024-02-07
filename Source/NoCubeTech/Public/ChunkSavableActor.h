// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "GlobalChunkRegistry.h"
#include "ChunkSavableActor.generated.h"

USTRUCT()
struct FSavedActorContainer {
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	UClass* ActorClass;

	UPROPERTY(SaveGame)
	FString ActorName;

	UPROPERTY(SaveGame)
	FTransform ActorTransform;

	UPROPERTY(SaveGame)
	TArray<uint8> BinaryData;
};

UENUM()
enum AChunkSavableActorState {
	NOTHING_NEEDED,
	END_SAVE_NEEDED,
	DELETE_NEEDED,
	INIT_NEEDED,
	LOAD_NEEDED, // object will wait for outer load call
	LOAD_OR_INIT_NEEDED
};

UCLASS()
class NOCUBETECH_API AChunkSavableActor : public AActor
{
	GENERATED_BODY()

private:
	FVector prevPositionForFindingChunk = FVector::ZeroVector;

	TWeakObjectPtr<AActor> currentChunkAnchor;
	TWeakObjectPtr<AGlobalChunkRegistry> chunkRegistry;

	//void findChunkAnchorIfNecessarily(bool switchToAnotherIfPositionInvalid);

	bool foundChunkAnchorWithValidPosition();

	AChunkSavableActorState state = LOAD_OR_INIT_NEEDED;
	
	UPROPERTY()
	uint64 chunkSavedActorUniqueId;

	void SetAsPreparedToSave(AActor* chunkAnchor);

protected:
	virtual void loadFromArchive(FArchive& archive) {};
	virtual void saveToArchive(FArchive& archive) {};
	virtual void setupByDefault() {};

public:

	void SetupByLoading(FArchive& archive) {
		loadFromArchive(archive);
		if (CanMoveBetweenChunks()) {
			prevPositionForFindingChunk = GetActorLocation();
		}
		state = NOTHING_NEEDED;
	}

	// call virtual setupByDefault and set state to END_SAVE_NEEDED
	void SetupByDefault()
	{
		setupByDefault();
		state = END_SAVE_NEEDED;
	};

	virtual void Tick(float DeltaSeconds) override;


	/*
	* Loads chunk save data from the disk, adds the record to array, saves chunk data to the list. 
	* Use in case when object is needed to be saved immediately (probably being destroyed), but chunk is not loaded.
	* Warning! Trying to save an object using this to chunk data that already contains its record will lead to object doubling.
	*/
	bool SaveToChunkDataDirectly();

	// Checks if the anchor is not null, is of AChunkAnchor class and GetActorLocation() is in chunk bounds
	bool CanBeSavedToChunk(AActor* chunkAnchor);

	void DeleteFromChunkAnchorIfNeeded(AActor* chunkAnchor);

	void SetSaveNeeded() {
		state = END_SAVE_NEEDED;
	}

	virtual bool CanMoveBetweenChunks() { return false; }

	bool IsSavableReady() {
		return state == NOTHING_NEEDED || state == END_SAVE_NEEDED ||
	}
};
