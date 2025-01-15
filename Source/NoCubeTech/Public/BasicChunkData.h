// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkSavableActor.h"
#include "ChunkSave.h"
#include "BasicChunkData.generated.h"

UCLASS()
class NOCUBETECH_API UBasicChunkData : public UUniversalChunkDataContainer
{
	GENERATED_BODY()
public:
	UBasicChunkData();
	~UBasicChunkData();

	UPROPERTY();
	TMap<int64, FSavedActorContainer> SavedActors = {};

	UPROPERTY();
	bool dataCreated = false;;

	bool IsDataCreated() {
		return dataCreated;
	}

	void SetDataCreated() {
		if (!dataCreated) {
			dataCreated = true;
			SetChunkDataChanged();
		}
	}

	void PutActor(int64 id, FSavedActorContainer& container) {
		SavedActors.Add(id, container);
		SetChunkDataChanged();

		// TODO: delete printing stuff
		//FString str = TEXT("Put actor to chunk data ");
		//str.AppendInt(id);
		//GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, str);
	}

	auto CreateActorsConstIterator() {
		return SavedActors.CreateConstIterator();
	}

	void RemoveActor(int64 id) {
		SavedActors.Remove(id);
		SetChunkDataChanged();

		// TODO: delete printing stuff

		FString str = TEXT("Removed actor from chunk data ");
		str.AppendInt(id);
		//GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Green, str);
	}

	bool ContainsActor(int64 id) {
		return SavedActors.Contains(id);
	}

	FSavedActorContainer GetActor(int64 id) {
		return SavedActors[id];
	}



};
