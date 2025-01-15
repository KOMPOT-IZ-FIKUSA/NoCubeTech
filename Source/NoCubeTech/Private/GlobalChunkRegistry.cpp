// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalChunkRegistry.h"
#include "Kismet/KismetTextLibrary.h"
#include "ChunkAnchorActor.h"
#include "ChunkSave.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGlobalChunkRegistry::AGlobalChunkRegistry()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.1);
	loadedAnchors = FActorGrid();
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

bool AGlobalChunkRegistry::CircleCollidesWithSquare(float circleX, float circleY, float radius, float squareX, float squareY, float squareSize) {
	float squareHalf = squareSize / 2;
	//float radiusHalf = radius / 2;
	float minX = squareX - squareHalf;
	float maxX = squareX + squareHalf;
	float minY = squareY - squareHalf;
	float maxY = squareY + squareHalf;
	// if center of the circle is inside of the square
	if (minX <= circleX && circleX <= maxX && minY <= circleY && circleY <= maxY) {
		return true;
	}
	float deltaXMinSqr = (minX - circleX) * (minX - circleX);
	float deltaXMaxSqr = (maxX - circleX) * (maxX - circleX);
	float deltaYMinSqr = (minY - circleY) * (minY - circleY);
	float deltaYMaxSqr = (maxY - circleY) * (maxY - circleY);
	float closestCornerX = fminf(deltaXMinSqr, deltaXMaxSqr);
	float closestCornerY = fminf(deltaYMinSqr, deltaYMaxSqr);
	if (sqrtf(closestCornerX + closestCornerY) <= radius) {
		return true;
	}
	if (circleX >= maxX && circleY <= maxY && circleY >= minY) {
		return circleX - maxX <= radius;
	}
	if (circleX <= minX && circleY <= maxY && circleY >= minY) {
		return minX - circleX <= radius;
	}
	if (circleY >= maxY && circleX <= maxX && circleX >= minX) {
		return circleY - maxY <= radius;
	}
	if (circleY <= minY && circleX <= maxX && circleX >= minX) {
		return minY - circleY <= radius;
	}
	return false;

}

bool AGlobalChunkRegistry::trySaveChunkData(UChunkSaveData* chunkDataInstance, int32 x, int32 y) {
	return chunkDataInstance->SaveToSlotIfDirty(GetChunkDataSaveSlotName(x, y));
}

TArray<FVector> AGlobalChunkRegistry::getAllPlayersPositions() {
	TArray<AActor*> players = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), players);
	TArray<FVector> playersPositions = TArray<FVector>();
	for (AActor* player : players) {
		APlayerController* player1 = Cast<APlayerController>(player);
		APawn* controlledPawn = player1->GetPawn();
		if (!controlledPawn) {
			continue;
		}
		playersPositions.Add(controlledPawn->GetActorLocation());
	}
	return playersPositions;
}

void AGlobalChunkRegistry::spawnChunkAnchor(int32 xIndex, int32 yIndex) {
	FActorSpawnParameters parameters = FActorSpawnParameters();
	FString name = TEXT("");
	name.Appendf(TEXT("ChunkAnchor_%d_%d"), xIndex, yIndex);
	parameters.Name = FName(name);
	parameters.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	FTransform transform = FTransform(FVector(IndexToPosition(xIndex), IndexToPosition(yIndex), 0));
	AChunkAnchor* spawnedAnchor = GetWorld()->SpawnActor<AChunkAnchor>(AChunkAnchor::StaticClass(), transform, parameters);
	loadedAnchors.Put(xIndex, yIndex, spawnedAnchor);
	spawnedAnchor->postInitWithChunkRegistry(this);
	spawnedAnchor->SetFolderPath(FName(TEXT("ChunkAnchors")));
}

void AGlobalChunkRegistry::destroyChunkAnchor(int32 xIndex, int32 yIndex) {
	TWeakObjectPtr<AActor> anchor = loadedAnchors.Get(xIndex, yIndex);
	if (anchor.IsValid()) {
		anchor->Destroy();
		loadedAnchors.CheckAndRemove(xIndex, yIndex);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to destroy invalid chunk anchor in AGlobalChunkRegistry::destroyChunkAnchor"));
	}
}

void AGlobalChunkRegistry::updateLoadedChunkAnchors() {
	// This variable defines the maximum half of the side of the square that contains a circle of player's load range (WorldPartitionLoadRange)
	// and measured in integer units, each equal WorldPartitionCellSize
	int maximumRangeInteger = ceilf(WorldPartitionLoadRange / WorldPartitionCellSize);
	for (FVector& playerPos : getAllPlayersPositions()) {
		int playerChunkIndexX = PositionToIndex(playerPos.X);
		int playerChunkIndexY = PositionToIndex(playerPos.Y);
		for (int xi = playerChunkIndexX - maximumRangeInteger; xi < playerChunkIndexX + maximumRangeInteger + 1; xi++) {
			for (int yi = playerChunkIndexY - maximumRangeInteger; yi < playerChunkIndexY + maximumRangeInteger + 1; yi++) {
				if (
					CircleCollidesWithSquare(playerPos.X, playerPos.Y, WorldPartitionLoadRange, IndexToPosition(xi), IndexToPosition(yi), WorldPartitionCellSize)
					&& !loadedAnchors.Contains(xi, yi)
					)
				{
					spawnChunkAnchor(xi, yi);
				}
			}
		}
	}
	// For each loaded chunk check if it needs to be unloaded by iterating over all the players
	for (auto It = loadedAnchors.CreateConstIterator(); It; ++It) {
		if (!It.Value().IsValid()) continue;
		int xi = It.Key().x;
		int yi = It.Key().y;
		bool somebodyStillClose = false;
		for (FVector& playerPos : getAllPlayersPositions()) {
			{
				if (CircleCollidesWithSquare(playerPos.X, playerPos.Y, WorldPartitionLoadRange + ChunkUnloadThresholdDistance, IndexToPosition(xi), IndexToPosition(yi), WorldPartitionCellSize))
				{
					somebodyStillClose = true;
					break;
				}
			}
		}
		AActor* chunkAnchor = It.Value().Get();
		if (!somebodyStillClose) {
			destroyChunkAnchor(xi, yi);
		}
	}
}

void AGlobalChunkRegistry::updateLoadedChunkData() {
	int savingOperationsDone = 0;
	TArray<FIntegerPair> chunkDataKeysToRemove = TArray<FIntegerPair>();
	float secondsNow = UGameplayStatics::GetTimeSeconds(GetWorld());
	for (auto It = loadedChunkData.CreateConstIterator(); It; ++It) {
		FIntegerPair key = It.Key();
		UChunkSaveData* savedInstance = Cast<UChunkSaveData>(It.Value());
		if (!savedInstance) {
			chunkDataKeysToRemove.Add(key);
			continue;
		}
		if (!savedInstance->IsChunkDataValid()) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Invalid chunk save instance being handled during AGlobalChunkRegistry::updateLoadedChunkData."));
			chunkDataKeysToRemove.Add(key);
			continue;
		}
		// handle unloading: if needed, save, remove and invalidate (without saving because it was already saved)
		if (secondsNow - savedInstance->GetLastAccessedSeconds() > ChunkDataUnloadIntervalSeconds) {
			if (trySaveChunkData(savedInstance, key.x, key.y)) {
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Saved chunk data"));
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("Cannot save chunk data instance"));
			}
			// When called trySaveChunkData, consider operation as performed. Then, if saved too much, stop saving for this frame
			savingOperationsDone += 1;
			if (savingOperationsDone > SavingAndLoadingOperationsPerFrameLimit) {
				break;
			}

			chunkDataKeysToRemove.Add(key);
			savedInstance->Invalidate();
			continue;
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
	updateLoadedChunkAnchors();
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


TWeakObjectPtr<AActor> AGlobalChunkRegistry::GetLoadedChunk(float worldX, float worldY) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	return loadedAnchors.Get(x, y);
}



UUniversalChunkDataContainer* AGlobalChunkRegistry::GetChunkData(float worldX, float worldY, FString key, UClass* class_) {
	int x = PositionToIndex(worldX);
	int y = PositionToIndex(worldY);
	UObject* data1 = loadedChunkData.Get(x, y);
	if (data1) {
		return Cast<UChunkSaveData>(data1)->GetDataContainer(key, class_);
	}
	// absent in grid
	// try load sync
	FString slotName = GetChunkDataSaveSlotName(x, y);
	UChunkSaveData* chunkDataInstance = Cast<UChunkSaveData>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
	if (chunkDataInstance) {
		// loaded existing, save to grid
		chunkDataInstance->SetUp(GetWorld());
		loadedChunkData.Put(x, y, chunkDataInstance);
		return chunkDataInstance->GetDataContainer(key, class_);
	}
	// cannot load, try to create
	chunkDataInstance = Cast<UChunkSaveData>(UGameplayStatics::CreateSaveGameObject(UChunkSaveData::StaticClass()));
	if (chunkDataInstance) {
		// created new, save to grid
		chunkDataInstance->SetUp(GetWorld());
		loadedChunkData.Put(x, y, chunkDataInstance);
		return chunkDataInstance->GetDataContainer(key, class_);
	}
	// in case of failure searching loaded, loading, creating new
	FString logStr = TEXT("Cannot find, load or create UChunkSaveData instance for chunk");
	logStr.Appendf(TEXT(" (%d, %d)"), x, y);
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, logStr);
	UE_LOG(LogTemp, Warning, TEXT("Cannot find, load or create UChunkSaveData instance for chunk"));
	return nullptr;
}

void AGlobalChunkRegistry::SetSavableActorTracked(int64 id) {
	trackedSavableActors.Put(id, true);
}

void AGlobalChunkRegistry::SetSavableActorUntracked(int64 id) {
	trackedSavableActors.Put(id, false);

}

bool AGlobalChunkRegistry::IsSavableActorTracked(int64 id) {
	return trackedSavableActors.Get(id);
}