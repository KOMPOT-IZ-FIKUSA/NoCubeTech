// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "ChunkSavableActor.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSave.generated.h"

UCLASS()
class NOCUBETECH_API UChunkSaveData : public USaveGame
{
	GENERATED_BODY()
protected:
	bool isValid = true;
	bool changed = false;
	float lastSavedSeconds = 0;
	float lastChangedSeconds = 0;

	UPROPERTY()
	TMap<int64, FSavedActorContainer> SavedActors;

	UPROPERTY()
	bool dataCreated = false;

public:

	bool IsDataCreated() {
		return dataCreated;
	}

	void SetDataCreated(float worldTimeSeconds) {
		if (!dataCreated) {
			dataCreated = true;
			SetChanged(worldTimeSeconds);
		}
	}

	void PutActor(int64 id, FSavedActorContainer& container, float worldTimeSeconds) {
		SavedActors.Add(id, container);
		SetChanged(worldTimeSeconds);
		
		FString str = TEXT("Put actor to chunk data ");
		str.AppendInt(id);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, str);
	}

	auto CreateActorsConstIterator() {
		return SavedActors.CreateConstIterator();
	}

	void RemoveActor(int64 id, float worldTimeSeconds) {
		SavedActors.Remove(id);
		SetChanged(worldTimeSeconds);

		FString str = TEXT("Removed actor from chunk data ");
		str.AppendInt(id);
		GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, str);
	}

	bool ContainsActor(int64 id) {
		return SavedActors.Contains(id);
	}

	FSavedActorContainer GetActor(int64 id) {
		return SavedActors[id];
	}

	void Invalidate() {
		isValid = false;
	}

	bool IsValid() {
		return isValid;
	}

	void SetChanged(float worldTimeSeconds) {
		check(isValid);
		changed = true;
		lastChangedSeconds = worldTimeSeconds;
	}

	float GetLastChangedSeconds() {
		return lastChangedSeconds;
	}

	float GetLastSavedSeconds() {
		return lastSavedSeconds;
	}

	void SetSaved(float worldTimeSeconds) {
		changed = false;
		lastSavedSeconds = worldTimeSeconds;
	}
};
