// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalChunkRegistry.h"
#include "Kismet/KismetTextLibrary.h"

// Sets default values
AGlobalChunkRegistry::AGlobalChunkRegistry()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	createdAnhors = FBoolGrid();
	loadedAnchors = FActorGrid();
}

// Called when the game starts or when spawned
void AGlobalChunkRegistry::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGlobalChunkRegistry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGlobalChunkRegistry::RegisterCreatedChunk(float worldX, float worldY) {
	int xi = PositionToIndex(worldX);
	int yi = PositionToIndex(worldY);
	if (IsChunkCreated(worldX, worldY)) {
		FString msg = TEXT("");
		msg.Appendf(TEXT("Trying to register a chunk (%d, %d) that is already registered in AGlobalChunkRegistry::RegisterCreatedChunk"), xi, yi);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, msg);
		UE_LOG(LogTemp, Warning, TEXT("Trying to register a chunk that is already registered in AGlobalChunkRegistry::RegisterCreatedChunk"))
	}
	createdAnhors.Put(xi, yi, true);
}


void AGlobalChunkRegistry::RegisterLoadedChunk(float worldX, float worldY, AActor* chunkAnchor) {
	int xi = PositionToIndex(worldX);
	int yi = PositionToIndex(worldY);
	if (IsChunkLoaded(worldX, worldY)) {
		FString msg = TEXT("");
		msg.Appendf(TEXT("Trying to register a chunk (%d, %d) that is already registered in AGlobalChunkRegistry::RegisterLoadedChunk"), xi, yi);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, msg);
		UE_LOG(LogTemp, Warning, TEXT("Trying to register a chunk that is already registered in AGlobalChunkRegistry::RegisterLoadedChunk"))
	}
	
	if (chunkAnchor) {
		loadedAnchors.Put(xi, yi, chunkAnchor);
		createdAnhors.Put(xi, yi, true);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Tried to register a nullptr chunk anchor in AGlobalChunkRegistry::RegisterChunk"));
		UE_LOG(LogTemp, Warning, TEXT("Tried to register a nullptr chunk anchor in AGlobalChunkRegistry::RegisterChunk"))
	}
}

void AGlobalChunkRegistry::UnloadChunk(float worldX, float worldY) {
	int xi = PositionToIndex(worldX);
	int yi = PositionToIndex(worldY);
	loadedAnchors.CheckAndRemove(xi, yi);
}

bool AGlobalChunkRegistry::IsChunkLoaded(float worldX, float worldY) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	return loadedAnchors.Contains(x, y) && loadedAnchors.Get(x, y).IsValid();
}

bool AGlobalChunkRegistry::IsChunkCreated(float worldX, float worldY) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	return createdAnhors.Contains(x, y) && createdAnhors.Get(x, y);
}

TWeakObjectPtr<AActor> AGlobalChunkRegistry::GetLoadedChunk(float worldX, float worldY) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	return loadedAnchors.Get(x, y);
}