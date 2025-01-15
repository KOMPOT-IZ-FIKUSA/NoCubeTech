// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataStructures.h"
#include "ChunkSave.h"
#include "GlobalChunkRegistry.generated.h"

class UChunkSaveData;

// A global actor that stores pointers to every loaded chunk. Make sure that world partition origin is set to (0, 0)
UCLASS()
class NOCUBETECH_API AGlobalChunkRegistry : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalChunkRegistry();

	bool IsChunkLoaded(float worldX, float worldY);
	bool IsChunkLoaded(FVector pos) { return IsChunkLoaded(pos.X, pos.Y); };
	
	TWeakObjectPtr<AActor> GetLoadedChunk(float worldX, float worldY);
	TWeakObjectPtr<AActor> GetLoadedChunk(FVector pos) { return GetLoadedChunk(pos.X, pos.Y); };
	
	UPROPERTY(EditAnywhere)
	float WorldPartitionCellSize = 6400;
	
	UPROPERTY(EditAnywhere)
	float WorldPartitionLoadRange = 12800;

	// index might be below zero
	static int PositionToIndex(float position, float WorldPartitionCellSize) {
		return floor(position / WorldPartitionCellSize + 0.5);
	}

	// index might be below zero
	int PositionToIndex(float position) {
		return floor(position / WorldPartitionCellSize + 0.5);
	}


	float IndexToPosition(int index) {
		return index * WorldPartitionCellSize;
	}

	void UnloadChunk(float worldX, float worldY);

	UUniversalChunkDataContainer* GetChunkData(float worldX, float worldY, FString key, UClass* class_);
	
	
	FString GetChunkDataSaveSlotName(int32 xIndex, int32 yIndex) {
		FString slotNameString = TEXT("");
		slotNameString.Appendf(TEXT("chunk_%d_%d"), xIndex, yIndex);
		return slotNameString;
	}

	const float ChunkDataUnloadIntervalSeconds = 120;
	const float ChunkUnloadThresholdDistance = 5000;
	const int SavingAndLoadingOperationsPerFrameLimit = 3;

	bool AreTwoPositionsInOneChunk(FVector pos1, FVector pos2) {
		int x1 = PositionToIndex(pos1.X);
		int y1 = PositionToIndex(pos1.Y);
		int x2 = PositionToIndex(pos2.X);
		int y2 = PositionToIndex(pos2.Y);
		return x1 == x2 && y1 == y2;
	}


	bool IsSavableActorTracked(int64 id);

	void SetSavableActorTracked(int64 id);

	void SetSavableActorUntracked(int64 id);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<FVector> getAllPlayersPositions();

	UPROPERTY()
	FActorGrid loadedAnchors;

	UPROPERTY()
	FObjectGrid loadedChunkData;

	FInt64BoolMap trackedSavableActors;

	static bool CircleCollidesWithSquare(float circleX, float circleY, float radius, float squareX, float squareY, float squareSize);

	bool trySaveChunkData(UChunkSaveData* chunkDataInstance, int32 xIndex, int32 yIndex);

	// save, remove, invalidate loaded instances if necessary
	void updateLoadedChunkData();

	// create and remove chunk anchors if necessary
	void updateLoadedChunkAnchors();

	void spawnChunkAnchor(int32 xIndex, int32 yIndex);
	void destroyChunkAnchor(int32 xIndex, int32 yIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
