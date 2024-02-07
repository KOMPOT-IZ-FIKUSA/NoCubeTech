// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkAnchorActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LandscapeTileManager.h"
#include "ChunkAdditionalObjectsGenerator.h"
#include "ChunkSave.h"
#include "ChunkSavableActor.h"
#include "GlobalChunkRegistry.h"

// Sets default values
AChunkAnchor::AChunkAnchor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DefaultSceneComponent")));
	SetRootComponent(rootSceneComponent);
	createdOrRestored = false;
	chunkRegistry = nullptr;
	actorsPreparedToSave = TMap<int, FSavedActorContainer>();
}


// Called when the game starts or when spawned
void AChunkAnchor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickInterval(0.5);
}

// Called every frame
void AChunkAnchor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (chunkRegistry.IsValid()) {
		CreateNeighbourChunksIfNeccesarily();
		loadContentOrInitIfNeccesarily();
	}
	else {
		TryFindChunkRegistry();
		if (chunkRegistry.IsValid()) {
			// was not found but now is found
			handleChunkRegistryFound();
		}
	}

}

void AChunkAnchor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (chunkRegistry.IsValid()) {
		float x = RootComponent->GetComponentLocation().X;
		float y = RootComponent->GetComponentLocation().Y;
		chunkRegistry->UnloadChunk(x, y);
	}

	saveAllData();

}

bool AChunkAnchor::IsPositionInBounds(float x, float y) {
	if (chunkRegistry.IsValid()) {
		float selfX = RootComponent->GetComponentLocation().X;
		float selfY = RootComponent->GetComponentLocation().Y;
		float selfMinX = selfX - chunkRegistry->WorldPartitionCellSize / 2;
		float selfMaxX = selfX + chunkRegistry->WorldPartitionCellSize / 2;
		float selfMinY = selfY - chunkRegistry->WorldPartitionCellSize / 2;
		float selfMaxY = selfY + chunkRegistry->WorldPartitionCellSize / 2;
		return
			x >= selfMinX && x < selfMaxX &&
			y >= selfMinY && y < selfMaxY;
	}
	return false;
}
bool AChunkAnchor::IsPositionInBounds(FVector pos) {
	return IsPositionInBounds(pos.X, pos.Y);
}

void AChunkAnchor::RemoveActorFromPreparedToSave(int actorId) {
	actorsPreparedToSave.Remove(actorId);
}

void AChunkAnchor::SetActorPreparedToSave(int actorId, FSavedActorContainer container) {
	actorsPreparedToSave.Add(actorId, container);
}

void AChunkAnchor::prepareAllActorsForSaving() {
	if (!chunkRegistry.IsValid()) {
		actorsPreparedToSave.Empty();
		return;
	}

	TArray<AActor*> actors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChunkSavableActor::StaticClass(), actors);

	for (AActor* actor : actors) {
		if (!actor->HasValidRootComponent()) {
			continue;
		}
		if (!IsPositionInBounds(actor->GetActorLocation())) {
			continue;
		}
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Trying to save actor from chunk anchor"));
		AChunkSavableActor* actor1 = Cast<AChunkSavableActor>(actor);
		actor1->SaveToChunk(this);
		actor1->DeleteFromChunkAnchorIfNeeded(this);
	}

	saveChunkInstance->SavedActors = TArray<FSavedActorContainer>();
	actorsPreparedToSave.GenerateValueArray(saveChunkInstance->SavedActors);

}

void AChunkAnchor::saveAllData() {
	if (chunkRegistry.IsValid()) {
		float x = RootComponent->GetComponentLocation().X;
		float y = RootComponent->GetComponentLocation().Y;
		int xi = chunkRegistry->PositionToIndex(x);
		int yi = chunkRegistry->PositionToIndex(y);

		FString slotNameString = getSaveSlotName();
		if (UChunkSaveData* saveChunkInstance = Cast<UChunkSaveData>(UGameplayStatics::CreateSaveGameObject(UChunkSaveData::StaticClass())))
		{


			if (UGameplayStatics::SaveGameToSlot(saveChunkInstance, slotNameString, 0))
			{
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, slotNameString);
				actorsPreparedToSave.Empty();
			}
			else {
				FString str = TEXT("Cannot save chunk ");
				str.Append(slotNameString);
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, str);
			}
		}
		else {
			FString str = TEXT("Cannot create chunk save ");
			str.Append(slotNameString);
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, str);
		}
	}
}

void AChunkAnchor::handleChunkRegistryFound() {



	setSelfLabel();
	FVector selfLocation = rootSceneComponent->GetComponentLocation();
	float x = selfLocation.X;
	float y = selfLocation.Y;

	if (chunkRegistry->IsChunkLoaded(x, y)) {
		//FString str = TEXT("");
		//str.Appendf(TEXT("Chunk is already loaded during AChunkAnchor::BeginPlay, x = %f, y = %y"), x, y);
		//GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, str);
	}
	else {
		chunkRegistry->RegisterLoadedChunk(x, y, this);
	}

}

bool AChunkAnchor::TryFindChunkRegistry() {
	AActor* chunkRegistry_ = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalChunkRegistry::StaticClass());
	if (chunkRegistry_) {
		chunkRegistry = TWeakObjectPtr<AGlobalChunkRegistry>((AGlobalChunkRegistry*)chunkRegistry_);
		return true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot register chunk anchor"));
		UE_LOG(LogTemp, Warning, TEXT("Cannot register chunk anchor"))
			return false;
	}

}

void AChunkAnchor::setSelfLabel() {
	FVector selfLocation = rootSceneComponent->GetComponentLocation();
	int xi = chunkRegistry->PositionToIndex(selfLocation.X);
	int yi = chunkRegistry->PositionToIndex(selfLocation.Y);
	FString actorName = TEXT("ChunkAnchor_");
	actorName.Appendf(TEXT("%d_%d"), xi, yi);
	SetActorLabel(actorName);
}

bool AChunkAnchor::CreateNeighbourChunksIfNeccesarily() {
	float x = RootComponent->GetComponentLocation().X;
	float y = RootComponent->GetComponentLocation().Y;

	float s = chunkRegistry->WorldPartitionCellSize;
	bool success = true;
	success &= CreateChunkIfNeccesary(x + s, y);
	success &= CreateChunkIfNeccesary(x - s, y);
	success &= CreateChunkIfNeccesary(x, y + s);
	success &= CreateChunkIfNeccesary(x, y - s);
	return success;

}

bool AChunkAnchor::CreateChunkIfNeccesary(float worldX, float worldY) {
	if (!chunkRegistry.IsValid()) {
		return false;
	}
	if (chunkRegistry->IsChunkCreated(worldX, worldY)) {
		return true;
	}

	// Create a level
	bool success;
	FString name = TEXT("");
	name.Appendf(TEXT("ChunkLevelInstance_%d_%d"), chunkRegistry->PositionToIndex(worldX), chunkRegistry->PositionToIndex(worldY));
	ULevelStreamingDynamic* level = ULevelStreamingDynamic::LoadLevelInstance(GetWorld(), "ChunkLevel", FVector(worldX, worldY, 0), FRotator::ZeroRotator, success, name);
	if (!success) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot load chunk level in AChunkAnchor::CreateChunkIfNeccesary"));
		UE_LOG(LogTemp, Warning, TEXT("Cannot load chunk level in AChunkAnchor::CreateChunkIfNeccesary"));
		return false;
	}
	else {
		chunkRegistry->RegisterCreatedChunk(worldX, worldY);
		return true;
	}

}


FString AChunkAnchor::GetSaveSlotName(float worldPartitionCellSize, float x, float y) {
	int xi = AGlobalChunkRegistry::PositionToIndex(x, worldPartitionCellSize);
	int yi = AGlobalChunkRegistry::PositionToIndex(y, worldPartitionCellSize);
	FString slotNameString = TEXT("");
	slotNameString.Appendf(TEXT("chunk_%d_%d"), xi, yi);
	return slotNameString;
}

FString AChunkAnchor::getSaveSlotName() {
	check(chunkRegistry.IsValid());
	float x = RootComponent->GetComponentLocation().X;
	float y = RootComponent->GetComponentLocation().Y;
	return GetSaveSlotName(chunkRegistry->WorldPartitionCellSize, x, y);
}


void AChunkAnchor::loadContentOrInitIfNeccesarily() {
	if (createdOrRestored) {
		return;
	}
	FString slotName = getSaveSlotName();

	if (UChunkSaveData* loadedGame = Cast<UChunkSaveData>(UGameplayStatics::LoadGameFromSlot(slotName, 0)))
	{
		// successfully loaded
	}
	else {
		initializeContent();
	}

}

void AChunkAnchor::initializeContent() {
	/*
	FVector selfLocation1 = rootSceneComponent->GetComponentLocation();
	float x1 = selfLocation1.X;
	float y1 = selfLocation1.Y;
	FString str1 = TEXT("");
	str1.Appendf(TEXT("initializeContent, x = %f, y = %f"), x1, y1);
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, str1);

	FVector selfLocation = rootSceneComponent->GetComponentLocation();
	int xi = chunkRegistry->PositionToIndex(selfLocation.X);
	int yi = chunkRegistry->PositionToIndex(selfLocation.Y);
	FActorSpawnParameters parameters;

	//parameters.OverrideLevel = GetLevel();
	parameters.Owner = this;

	FString actorName = TEXT("LandscapeTileActor_");
	actorName.Appendf(TEXT("%d_%d"), xi, yi);
	parameters.Name = FName(actorName);
	AActor* landscapeActor = GetWorld()->SpawnActor<AActor>(parameters);
	landscapeActor->SetActorLabel(actorName);
	ULandscapeTileManager* landscapeTile = NewObject<ULandscapeTileManager>(landscapeActor);

	landscapeTile->RegisterComponent();
	landscapeActor->AddInstanceComponent(landscapeTile);
	landscapeActor->SetRootComponent(landscapeTile);
	landscapeTile->SetWorldLocation(selfLocation);
	landscapeTile->TileSize = chunkRegistry->WorldPartitionCellSize;
	landscapeTile->trackedPawns.Add((APawn*)UGameplayStatics::GetActorOfClass(GetWorld(), APawn::StaticClass()));
	landscapeTile->DetailsDistanceUnit = chunkRegistry->WorldPartitionCellSize * 1.2;
	landscapeTile->MaxTileDetails = chunkRegistry->WorldPartitionCellSize / 1600;
	landscapeTile->PixelsBySquare = 8;
	landscapeTile->chunkX = xi;
	landscapeTile->chunkY = yi;
	*/

	/*
	parameters.OverrideLevel = GetLevel();
	parameters.Owner = this;
	actorName = TEXT("ChunkAddObjGenerator_");
	actorName.Appendf(TEXT("%d_%d"), xi, yi);
	parameters.Name = FName(actorName);
	AChunkAdditionalObjectsGenerator* additionalObjectsGeneratorActor = GetWorld()->SpawnActor<AChunkAdditionalObjectsGenerator>(parameters);
	additionalObjectsGeneratorActor->SetActorLabel(actorName);
	USceneComponent* sceneComponentChunkObjectsGenerator = NewObject<USceneComponent>(additionalObjectsGeneratorActor);
	sceneComponentChunkObjectsGenerator->RegisterComponent();
	additionalObjectsGeneratorActor->AddInstanceComponent(sceneComponentChunkObjectsGenerator);
	additionalObjectsGeneratorActor->SetRootComponent(sceneComponentChunkObjectsGenerator);
	sceneComponentChunkObjectsGenerator->SetWorldLocation(selfLocation);
	additionalObjectsGeneratorActor->chunkX = xi;
	additionalObjectsGeneratorActor->chunkY = yi;
	*/


}