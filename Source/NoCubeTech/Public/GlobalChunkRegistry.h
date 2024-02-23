// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataStructures.h"
#include "GlobalChunkRegistry.generated.h"


// A global actor that stores pointers to every loaded chunk. Make sure that world partition origin is set to (0, 0)
UCLASS()
class NOCUBETECH_API AGlobalChunkRegistry : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalChunkRegistry();

	// Called when AChunkAnchor BeginPlay is executed
	void RegisterLoadedChunk(float worldX, float worldY, AActor* chunkAnchor);

	// Called when created a level containing the anchor
	void RegisterCreatedChunk(float worldX, float worldY);

	bool IsChunkLoaded(float worldX, float worldY);
	bool IsChunkLoaded(FVector pos) { return IsChunkLoaded(pos.X, pos.Y); };
	bool IsChunkCreated(float worldX, float worldY);
	
	TWeakObjectPtr<AActor> GetLoadedChunk(float worldX, float worldY);
	TWeakObjectPtr<AActor> GetLoadedChunk(FVector pos) { return GetLoadedChunk(pos.X, pos.Y); };
	
	TWeakObjectPtr<AActor> GetLoadedOrUnloadedChunk(float worldX, float worldY);
	TWeakObjectPtr<AActor> GetLoadedOrUnloadedChunk(FVector pos) { return GetLoadedOrUnloadedChunk(pos.X, pos.Y); };
	
	
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

	UObject* GetChunkData(float worldX, float worldY);
	
	UObject* GetChunkData(FVector pos);
	
	FString GetChunkDataSaveSlotName(int32 xIndex, int32 yIndex) {
		FString slotNameString = TEXT("");
		slotNameString.Appendf(TEXT("chunk_%d_%d"), xIndex, yIndex);
		return slotNameString;
	}

	const float ChunkDataSaveIntervalSeconds = 15;
	const float ChunkDataUnloadIntervalSeconds = 120;

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

	UPROPERTY()
	FActorGrid loadedAnchors;

	UPROPERTY()
	FActorGrid loadedOrUnloadedAnchors;

	UPROPERTY()
	FBoolGrid createdAnhors;

	UPROPERTY()
	FObjectGrid loadedChunkData;

	FInt64BoolMap trackedSavableActors;


	bool trySaveChunkData(UObject* chunkDataInstance, int32 xIndex, int32 yIndex);

	// save, remove, invalidate loaded instances if necessarily
	void updateLoadedChunkData();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
