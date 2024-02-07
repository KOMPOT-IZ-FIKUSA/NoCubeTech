// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkSavableActor.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSave.h"
#include "ChunkAnchorActor.h"

void AChunkSavableActor::SetAsPreparedToSave(AActor* chunkAnchor) {
	check(chunkAnchor != nullptr);
	AChunkAnchor* chunkAnchor1 = Cast<AChunkAnchor>(chunkAnchor);
	check(chunkAnchor1 != nullptr);
	FSavedActorContainer container = FSavedActorContainer();
	container.ActorClass = GetClass();
	container.ActorName = GetName();
	container.ActorTransform = GetTransform();
	container.BinaryData = TArray<uint8>();
	FArchive archive = FArchive();
	saveToArchive(archive);
	chunkAnchor1->SetActorPreparedToSave(GetUniqueID(), container);
}

bool AChunkSavableActor::CanBeSavedToChunk(AActor* chunkAnchor) {
	AChunkAnchor* chunkAnchor1 = Cast<AChunkAnchor>(chunkAnchor);
	if (chunkAnchor1 == nullptr) return false;
	return chunkAnchor1->IsPositionInBounds(GetActorLocation());
}

bool AChunkSavableActor::SaveToChunkDataDirectly() {
	if (!chunkRegistry.IsValid()) {
		return false;
	}
	if (!HasValidRootComponent()) {
		return false;
	}
	FVector selfLocation = GetActorLocation();
	FString slotName = AChunkAnchor::GetSaveSlotName(chunkRegistry->WorldPartitionCellSize, selfLocation.X, selfLocation.Y);
	UChunkSaveData* chunkDataInstance = Cast<UChunkSaveData>(UGameplayStatics::LoadGameFromSlot(slotName, 0));
	if (!chunkDataInstance)
	{
		chunkDataInstance = Cast<UChunkSaveData>(UGameplayStatics::CreateSaveGameObject(UChunkSaveData::StaticClass()));
	}
	if (!chunkDataInstance) {
		return false;
	}

	FSavedActorContainer container = FSavedActorContainer();
	container.ActorClass = GetClass();
	container.ActorName = GetName();
	container.ActorTransform = GetTransform();
	container.BinaryData = TArray<uint8>();
	FArchive archive = FArchive();
	saveToArchive(archive);
	chunkDataInstance->SavedActors.Add(container);

	if (UGameplayStatics::SaveGameToSlot(chunkDataInstance, slotName, 0))
	{
		return true;
	}
	else {
		return false;
	}
}


void AChunkSavableActor::DeleteFromChunkAnchorIfNeeded(AActor* chunkAnchor) {
	if (state != DELETE_NEEDED) {
		return;
	}
	AChunkAnchor* chunkAnchor1 = Cast<AChunkAnchor>(chunkAnchor);
	if (!chunkAnchor1) {
		return;
	}
	if (!chunkAnchor1->IsPositionInBounds(GetActorLocation())) {
		return;
	}

	chunkAnchor1->RemoveActorFromPreparedToSave(GetUniqueID());
	state = NOTHING_NEEDED;
}

bool AChunkSavableActor::foundChunkAnchorWithValidPosition() {
	return (currentChunkAnchor.IsValid() && Cast<AChunkAnchor>(currentChunkAnchor)->IsPositionInBounds(GetActorLocation()));
}

void AChunkSavableActor::findChunkAnchorIfNecessarily(bool switchToAnotherIfPositionInvalid) {
	if (currentChunkAnchor.IsValid()) {
		return;
	}
	if (!chunkRegistry.IsValid()) {
		AActor* globalChunkRegistry = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalChunkRegistry::StaticClass());
		chunkRegistry = TWeakObjectPtr<AGlobalChunkRegistry>(Cast<AGlobalChunkRegistry>(globalChunkRegistry));
	}
	if (!chunkRegistry.IsValid()) {
		return;
	}
	FVector selfLocation = GetActorLocation();
	if (!chunkRegistry->IsChunkLoaded(selfLocation)) {
		return;
	}
	currentChunkAnchor = chunkRegistry->GetLoadedChunk(selfLocation); // still might not be valid
}

void AChunkSavableActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (state == INIT_NEEDED) {
		SetupByDefault();
	}
	else if (state == LOAD_NEEDED) {
		// Do nothing, wait for outer
	}
	else if (state == LOAD_OR_INIT_NEEDED) {
		// Do nothing, wait for outer to define how to start
	}

	/*
	findChunkAnchorIfNecessarily(false);
	if (CanMoveBetweenChunks() && currentChunkAnchor.IsValid() && chunkRegistry.IsValid()) {
		if (capturePosition) {
			FVector currentPosition = GetActorLocation();
			int currentXIndex = chunkRegistry->PositionToIndex(currentPosition.X);
			int currentYIndex = chunkRegistry->PositionToIndex(currentPosition.Y);
			int prevXIndex = chunkRegistry->PositionToIndex(prevPositionForFindingChunk.X);
			int prevYIndex = chunkRegistry->PositionToIndex(prevPositionForFindingChunk.Y);

			if (currentXIndex != prevXIndex || currentYIndex != prevYIndex) {
				AChunkAnchor* anchorOld = Cast<AChunkAnchor>(currentChunkAnchor);
				anchorOld->RemoveActorFromPreparedToSave(GetUniqueID());
				findChunkAnchorIfNecessarily(true); // changes currentChunkAnchor
				AChunkAnchor* anchorNew = Cast<AChunkAnchor>(currentChunkAnchor);
				state = END_SAVE_NEEDED;
			}
		}
	}
	*/
}