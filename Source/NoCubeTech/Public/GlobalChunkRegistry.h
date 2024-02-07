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
	
	UPROPERTY(EditAnywhere)
	float WorldPartitionCellSize = 12800;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FActorGrid loadedAnchors;

	UPROPERTY()
	FBoolGrid createdAnhors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
