// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalChunkRegistry.h"
#include "Kismet/KismetTextLibrary.h"
#include "ChunkSave.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGlobalChunkRegistry::AGlobalChunkRegistry()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(10);
	createdAnhors = FBoolGrid();
	loadedAnchors = FActorGrid();
	loadedOrUnloadedAnchors = FActorGrid();
	loadedChunkData = FObjectGrid();
	trackedSavableActors = FInt64BoolMap();
}

// Called when the game starts or when spawned
void AGlobalChunkRegistry::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) {
		Destroy();
	}
	else {
	}

}

bool AGlobalChunkRegistry::trySaveChunkData(UObject* chunkDataInstance, int32 x, int32 y) {
	return UGameplayStatics::SaveGameToSlot(Cast<UChunkSaveData>(chunkDataInstance), GetChunkDataSaveSlotName(x, y), 0);
}

void AGlobalChunkRegistry::updateLoadedChunkData() {
	TArray<FIntegerPair> chunkDataKeysToRemove = TArray<FIntegerPair>();
	float secondsNow = UGameplayStatics::GetTimeSeconds(GetWorld());
	for (auto It = loadedChunkData.CreateConstIterator(); It; ++It) {
		FIntegerPair key = It.Key();
		UChunkSaveData* savedInstance = Cast<UChunkSaveData>(It.Value());
		if (!savedInstance) {
			chunkDataKeysToRemove.Add(key);
			continue;
		}
		check(savedInstance->IsValid()); // if not valid, it is set so from another place, which is an error
		// handle unloading: if needed, remove and invalidate (without saving because it was already saved)
		if (secondsNow - savedInstance->GetLastChangedSeconds() > ChunkDataUnloadIntervalSeconds) {
			chunkDataKeysToRemove.Add(key);
			savedInstance->Invalidate();
			continue;
		}
		// handle saving: if needed, save and set saved
		if (secondsNow - savedInstance->GetLastSavedSeconds() > ChunkDataSaveIntervalSeconds) {
			if (trySaveChunkData(savedInstance, key.x, key.y)) {
				//GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, TEXT("Saved all chunk data"));
				savedInstance->SetSaved(UGameplayStatics::GetTimeSeconds(GetWorld()));
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot save chunk data instance"));
			}
		}
	}
	for (FIntegerPair& key : chunkDataKeysToRemove) {
		loadedChunkData.CheckAndRemove(key);
	}
}

// Called every frame
void AGlobalChunkRegistry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	updateLoadedChunkData();
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
		loadedOrUnloadedAnchors.Put(xi, yi, chunkAnchor);
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

TWeakObjectPtr<AActor> AGlobalChunkRegistry::GetLoadedOrUnloadedChunk(float worldX, float worldY) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	return loadedOrUnloadedAnchors.Get(x, y);
}


UObject* AGlobalChunkRegistry::GetChunkData(FVector pos) {
	return GetChunkData(pos.X, pos.Y);
}

UObject* AGlobalChunkRegistry::GetChunkData(float worldX, float worldY) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	UObject* data1 = loadedChunkData.Get(x, y);
	if (data1) {
		return Cast<UChunkSaveData>(data1);
	}
	// absent in grid

	// try load sync
	FString slotName = GetChunkDataSaveSlotName(x, y);
	UChunkSaveData* chunkDataInstance = Cast<UChunkSaveData>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
	if (chunkDataInstance) {
		// loaded existing, save to grid
		loadedChunkData.Put(x, y, chunkDataInstance);
		return chunkDataInstance;
	}
	if (!chunkDataInstance)
	{
		// cannot load, try to create
		chunkDataInstance = Cast<UChunkSaveData>(UGameplayStatics::CreateSaveGameObject(UChunkSaveData::StaticClass()));
	}
	if (chunkDataInstance) {
		// created new, save to grid
		loadedChunkData.Put(x, y, chunkDataInstance);
		return chunkDataInstance;
	}
	// in case of failure searching loaded, loading, creating new
	FString logStr = TEXT("Cannot find, load or create UChunkSaveData instance for chunk");
	logStr.Appendf(TEXT(" (%d, %d)"), x, y);
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, logStr);
	UE_LOG(LogTemp, Warning, TEXT("Cannot find, load or create UChunkSaveData instance for chunk"));
	return nullptr;
}

void AGlobalChunkRegistry::SetSavableActorTracked(int64 id) {
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("AGlobalChunkRegistry::SetSavableActorTracked"));
	trackedSavableActors.Put(id, true);
}

void AGlobalChunkRegistry::SetSavableActorUntracked(int64 id) {
	trackedSavableActors.Put(id, false);
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("AGlobalChunkRegistry::SetSavableActorUntracked"));

}

bool AGlobalChunkRegistry::IsSavableActorTracked(int64 id) {
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("AGlobalChunkRegistry::IsSavableActorTracked"));
	return trackedSavableActors.Get(id);
}