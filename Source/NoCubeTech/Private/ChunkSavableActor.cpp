// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkSavableActor.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSave.h"
#include "ChunkAnchorActor.h"

AChunkSavableActor::AChunkSavableActor() {

	PrimaryActorTick.bCanEverTick = true;

	chunkSavedActorUniqueId = 0;
	chunkRegistry = nullptr;
	currentlySavedIndex = FSavedActorChunkSavedIndex();
	state = LOAD_OR_INIT_NEEDED;

	rootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("DefaultSceneComponent")));
	SetRootComponent(rootSceneComponent);
}

void AChunkSavableActor::fillContainer(FSavedActorContainer& container) {
	check(chunkSavedActorUniqueId != 0);
	container.ActorClass = GetClass();
	FString name = TEXT("SavableActor_");
	name.AppendInt(chunkSavedActorUniqueId);
	container.ActorName = name;
	container.ActorTransform = GetTransform();
	container.BinaryData = TArray<uint8>();
	container.ActorSavableUID = chunkSavedActorUniqueId;
	FMemoryWriter writer = FMemoryWriter(container.BinaryData);
	saveToArchive(writer);
}

bool AChunkSavableActor::TrySaveToChunkRegistryDataUniquely(FVector position) {
	if (!chunkRegistry.IsValid()) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to use invalid chunkRegistry in AChunkSavableActor::SaveToChunkRegistryData"));
		return false;
	}
	if (state != TRACKING) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to save actor with state != TRACKING in AChunkSavableActor::SaveToChunkRegistryData"));
		return false;
	}
	float now = UGameplayStatics::GetTimeSeconds(GetWorld());
	// was already saved
	if (currentlySavedIndex.saved) {

		if (chunkRegistry->AreTwoPositionsInOneChunk(currentlySavedIndex.position, position)) { // saving to the save chunk as last time
			UChunkSaveData* chunkSaveInstance = Cast<UChunkSaveData>(chunkRegistry->GetChunkData(position));
			if (!chunkSaveInstance) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("1 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
				return false;
			}
			FSavedActorContainer container = FSavedActorContainer();
			fillContainer(container);
			chunkSaveInstance->PutActor(chunkSavedActorUniqueId, container, now);
		}
		else { // saving to the different chunk
			UChunkSaveData* oldChunkSaveInstance = Cast<UChunkSaveData>(chunkRegistry->GetChunkData(currentlySavedIndex.position));
			if (!oldChunkSaveInstance) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("2 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
				return false;
			}
			UChunkSaveData* newChunkSaveInstance = Cast<UChunkSaveData>(chunkRegistry->GetChunkData(position));
			if (!newChunkSaveInstance) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("3 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
				return false;
			}
			FSavedActorContainer container = FSavedActorContainer();
			fillContainer(container);
			oldChunkSaveInstance->RemoveActor(chunkSavedActorUniqueId, now);
			newChunkSaveInstance->PutActor(chunkSavedActorUniqueId, container, now);
		}
	}
	// first save
	else {
		UChunkSaveData* chunkSaveInstance = Cast<UChunkSaveData>(chunkRegistry->GetChunkData(position));
		if (!chunkSaveInstance) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("4 Cannot get chunk data instance in AChunkSavableActor::SaveToChunkRegistryData"));
			return false;
		}
		FSavedActorContainer container = FSavedActorContainer();
		fillContainer(container);
		chunkSaveInstance->PutActor(chunkSavedActorUniqueId, container, now);
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

	UChunkSaveData* chunkSaveInstance = Cast<UChunkSaveData>(chunkRegistry->GetChunkData(currentlySavedIndex.position));
	if (!chunkSaveInstance) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot get chunk data instance in AChunkSavableActor::TryDeleteFromChunkRegistryData"));
		return false;
	}
	chunkSaveInstance->RemoveActor(chunkSavedActorUniqueId, UGameplayStatics::GetTimeSeconds(GetWorld()));
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

	if (state == NEED_TO_DESTROY_WITHOUT_HANDLING) {
		Destroy();
	}

	if (!chunkRegistry.IsValid()) {
		tryFindChunkRegistry();
		if (!chunkRegistry.IsValid()) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Yellow, TEXT("Cannot find chunk registry"));
			return;
		};
	}

	if (state == TRACKING_PAUSED_BY_STREAMING_UNLOAD) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("state = TRACKING_PAUSED_BY_STREAMING_UNLOAD in AChunkSavableActor::Tick"));
		return;
	}

	if (state == LOAD_OR_INIT_NEEDED) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("state = LOAD_OR_INIT_NEEDED in AChunkSavableActor::Tick"));
		return;
	}

	if (state == TRACKING) {
		// if chunk anchor is not valid
		if (!Owner || !Cast<AChunkAnchor>(Owner)->IsPositionInBounds(GetActorLocation())) {
			TrySaveToChunkRegistryDataUniquely(GetActorLocation());
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Bounds problem found. Solving..."));
			AChunkAnchor* chunk = Cast<AChunkAnchor>(chunkRegistry->GetLoadedChunk(GetActorLocation()).Get());
			if (chunk) {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Found another loaded chunk"));
				SetOwner(chunk);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("None chunk found"));
				state = NEED_TO_DESTROY_WITHOUT_HANDLING;
				Destroy();
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
}

void AChunkSavableActor::BeginPlay() {
	Super::BeginPlay();

	if (state == LOAD_OR_INIT_NEEDED) {
		// Do nothing
	}
	else if (state == TRACKING) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("AChunkSavableActor::BeginPlay with state = TRACKING ; Destroying..."));
		state = NEED_TO_DESTROY_WITHOUT_HANDLING;
		Destroy();
	}
	else if (state == TRACKING_PAUSED_BY_STREAMING_UNLOAD) {
		tryFindChunkRegistry();
		startTracking();
	}
	else if (state == NEED_TO_DESTROY_WITHOUT_HANDLING) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("AChunkSavableActor::BeginPlay with state = NEED_TO_DESTROY_WITHOUT_HANDLING ; Destroying..."));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("AChunkSavableActor::BeginPlay with unknown state ; Destroying..."));
		state = NEED_TO_DESTROY_WITHOUT_HANDLING;
		Destroy();
	}
}

void AChunkSavableActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);


	if (EndPlayReason == EEndPlayReason::RemovedFromWorld) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("RemovedFromWorld"));
	}
	if (EndPlayReason == EEndPlayReason::Destroyed) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Destroyed"));
	}

	return;
	GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("EndPlay!"));
	if (state == NEED_TO_DESTROY_WITHOUT_HANDLING) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Deleting without handling!"));
		return;
	}
	if (state == TRACKING) {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Pausing tracking!"));
		pauseTracking();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Invalid state being handled during AChunkSavableActor::EndPlay"));
	}


	if (state == TRACKING && chunkRegistry.IsValid()) {
		chunkRegistry->SetSavableActorUntracked(chunkSavedActorUniqueId);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::White, TEXT("Trying to save while EndPlay"));
		TrySaveToChunkRegistryDataUniquely(GetActorLocation());
	}
}

void AChunkSavableActor::startTracking() {
	check(chunkRegistry.IsValid());
	check(chunkSavedActorUniqueId != 0);
	if (state == LOAD_OR_INIT_NEEDED || state == TRACKING_PAUSED_BY_STREAMING_UNLOAD) {
		state = TRACKING;
		chunkRegistry->SetSavableActorTracked(chunkSavedActorUniqueId);
	}
	else if (state == TRACKING) {
		// already tracking, which means tihs object is a copy of another - needs to be deleted
		if (chunkRegistry->IsSavableActorTracked(chunkSavedActorUniqueId)) {
			state = NEED_TO_DESTROY_WITHOUT_HANDLING;
			Destroy();
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Invalid state in AChunkSavableActor::startTracking"));
	}
}

void AChunkSavableActor::pauseTracking() {
	check(chunkSavedActorUniqueId != 0);
	check(chunkRegistry.IsValid());
	
	if (state == TRACKING) {
		if (!chunkRegistry->IsSavableActorTracked(chunkSavedActorUniqueId)) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to start tracking an actor that is already being tracked"));
		}
		state = TRACKING_PAUSED_BY_STREAMING_UNLOAD;
		chunkRegistry->SetSavableActorUntracked(chunkSavedActorUniqueId);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Invalid state in AChunkSavableActor::pauseTracking"));
	}
}