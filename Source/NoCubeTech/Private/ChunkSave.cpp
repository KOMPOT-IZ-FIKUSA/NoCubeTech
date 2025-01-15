// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkSave.h"

void UUniversalChunkDataContainer::SetChunkDataChanged() {
	check(IsChunkDataValid());
	chunkDataOwner->SetDirty();
}

void UUniversalChunkDataContainer::SetChunkDataAccessed() {
	check(IsChunkDataValid());
	chunkDataOwner->UpdateAccessed();
}

bool UUniversalChunkDataContainer::IsChunkDataValid() {
	return chunkDataOwner != nullptr && chunkDataOwner->IsChunkDataValid();
}

void UUniversalChunkDataContainer::SetUp(UChunkSaveData* chunkDataOwner_)
{
	check(chunkDataOwner == nullptr);
	chunkDataOwner = chunkDataOwner_;
}
