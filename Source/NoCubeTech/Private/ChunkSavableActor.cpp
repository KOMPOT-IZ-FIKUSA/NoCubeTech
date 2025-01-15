// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkSavableActor.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSave.h"
#include "BasicChunkData.h"
#include "ChunkAnchorActor.h"

int SimpleStaticIncrementHelper::incrementingVariable = 0;

AChunkSavableActor::AChunkSavableActor() {

	PrimaryActorTick.bCanEverTick = true;

	chunkSavedActorUniqueId = 0;
	chunkRegistry = nullptr;
	currentlySavedIndex = FSavedActorChunkSavedIndex();

	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DefaultSceneComponent")));
	SetRootComponent(rootSceneComponent);
}

void AChunkSavableActor::fillContainer(FSavedActorContainer& container) {
	if (chunkSavedActorUniqueId == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to save zero-id AChunkSavableActor to the archive in AChunkSavableActor::fillContainer. Destroying..."));
		DestroySavable(FDestroySavableActorMode::NONE);
	}
	container.ActorClass = GetClass();
	FString name = TEXT("SavableActor_");
	name.AppendInt(chunkSavedActorUniqueId);
	container.ActorName = name;
	container.ActorTransform = GetTransform();
	container.BinaryData = TArray<uint8>();
	container.ActorSavableUID = chunkSavedActorUniqueId;
	FMemoryWriter writer = FMemoryWriter(container.BinaryData);
	writer << chunkSavedActorUniqueId;

	saveToArchive(writer);
}

bool AChunkSavableActor::TrySaveToChunkRegistryDataUniquely(FVector position) {
	if (!chunkRegistry.IsValid()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to use invalid chunkRegistry in AChunkSavableActor::SaveToChunkRegistryData"));
		return false;
	}
	if (chunkSavedActorUniqueId == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to save zero-id actor in AChunkSavableActor::SaveToChunkRegistryData"));
		return false;
	}
	float now = UGameplayStatics::GetTimeSeconds(GetWorld());
	// was already saved
	if (currentlySavedIndex.saved) {

		if (chunkRegistry->AreTwoPositionsInOneChunk(currentlySavedIndex.position, position)) { // saving to the save chunk as last time
			UBasicChunkData* chunkSaveInstance = Cast<UBasicChunkData>(chunkRegistry->GetChunkData(position.X, position.Y, TEXT("basic"), UBasicChunkData::StaticClass()));
			if (!chunkSaveInstance) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("1 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
				return false;
			}
			FSavedActorContainer container = FSavedActorContainer();
			fillContainer(container);
			chunkSaveInstance->PutActor(chunkSavedActorUniqueId, container);
		}
		else { // saving to the different chunk
			UBasicChunkData* oldChunkSaveInstance = Cast<UBasicChunkData>(chunkRegistry->GetChunkData(currentlySavedIndex.position.X, currentlySavedIndex.position.Y, TEXT("basic"), UBasicChunkData::StaticClass()));
			if (!oldChunkSaveInstance) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("2 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
				return false;
			}
			UBasicChunkData* newChunkSaveInstance = Cast<UBasicChunkData>(chunkRegistry->GetChunkData(position.X, position.Y, TEXT("basic"), UBasicChunkData::StaticClass()));
			if (!newChunkSaveInstance) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("3 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
				return false;
			}
			FSavedActorContainer container = FSavedActorContainer();
			fillContainer(container);
			oldChunkSaveInstance->RemoveActor(chunkSavedActorUniqueId);
			newChunkSaveInstance->PutActor(chunkSavedActorUniqueId, container);
		}
	}
	// first save
	else {
		UBasicChunkData* chunkSaveInstance = Cast<UBasicChunkData>(chunkRegistry->GetChunkData(position.X, position.Y, TEXT("basic"), UBasicChunkData::StaticClass()));
		if (!chunkSaveInstance) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("4 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
			return false;
		}
		FSavedActorContainer container = FSavedActorContainer();
		fillContainer(container);
		chunkSaveInstance->PutActor(chunkSavedActorUniqueId, container);
	}
	currentlySavedIndex.saved = true;
	currentlySavedIndex.position = position;
	return true;
}

bool AChunkSavableActor::TryDeleteFromChunkRegistryData() {
	if (!chunkRegistry.IsValid()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to use invalid chunkRegistry in AChunkSavableActor::TryDeleteFromChunkRegistryData"));
		return false;
	}
	if (!currentlySavedIndex.saved) {
		return false;
	}

	UBasicChunkData* chunkSaveInstance = Cast<UBasicChunkData>(chunkRegistry->GetChunkData(currentlySavedIndex.position.X, currentlySavedIndex.position.Y, TEXT("basic"), UBasicChunkData::StaticClass()));
	if (!chunkSaveInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot get chunk data instance in AChunkSavableActor::TryDeleteFromChunkRegistryData"));
		return false;
	}
	chunkSaveInstance->RemoveActor(chunkSavedActorUniqueId);
	currentlySavedIndex.saved = false;
	return true;
}

FString AChunkSavableActor::GetNameUsingId() {
	FString actorName = TEXT("SavableActor_");
	actorName.AppendInt(chunkSavedActorUniqueId);
	return actorName;
}

void AChunkSavableActor::tryFindChunkRegistry() {
	if (chunkRegistry.IsValid()) return;
	AGlobalChunkRegistry* found = Cast<AGlobalChunkRegistry>(UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalChunkRegistry::StaticClass()));
	chunkRegistry = found;
}



void AChunkSavableActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	if (!HasAuthority()) {
		return;
	}

	if (chunkSavedActorUniqueId == 0) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Called AChunkSavableActor::Tick for zero-id actor. Destroying..."));
		DestroySavable(FDestroySavableActorMode::NONE);
	}

	// if chunk anchor is not valid
	if (!Owner || Owner->IsActorBeingDestroyed() || !Owner->IsValidLowLevel() || !Cast<AChunkAnchor>(Owner)->IsPositionInBounds(GetActorLocation())) {
		TrySaveToChunkRegistryDataUniquely(GetActorLocation());
		AChunkAnchor* chunk = Cast<AChunkAnchor>(chunkRegistry->GetLoadedChunk(GetActorLocation()).Get());
		if (chunk) {
			SetOwner(chunk);
		}
		else {
			DestroySavable(FDestroySavableActorMode::NONE); // saved earlier
		}
	}
	else {
		// Do save over time
		if (lastSavedTime + GetSaveInterval() < UGameplayStatics::GetTimeSeconds(GetWorld())) {
			bool success = TrySaveToChunkRegistryDataUniquely(GetActorLocation());
			if (success) {
				lastSavedTime = UGameplayStatics::GetTimeSeconds(GetWorld());
			}
		}
	}
}

void AChunkSavableActor::BeginPlay() {
	Super::BeginPlay();
}

void AChunkSavableActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

}


void AChunkSavableActor::DestroySavable(FDestroySavableActorMode mode) {
	if (!chunkRegistry.IsValid() || chunkSavedActorUniqueId == 0) {

	}
	else if (mode == SAVE) {
		TrySaveToChunkRegistryDataUniquely(GetActorLocation());
		chunkRegistry->SetSavableActorUntracked(chunkSavedActorUniqueId);
	}
	else if (mode == DELETE) {
		TrySaveToChunkRegistryDataUniquely(GetActorLocation());
		chunkRegistry->SetSavableActorUntracked(chunkSavedActorUniqueId);
	}
	else if (mode == NONE) {
		chunkRegistry->SetSavableActorUntracked(chunkSavedActorUniqueId);
	}
	Destroy();
}
