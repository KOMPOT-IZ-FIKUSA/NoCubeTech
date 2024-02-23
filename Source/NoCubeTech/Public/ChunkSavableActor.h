// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "GlobalChunkRegistry.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSavableActor.generated.h"

USTRUCT()
struct FSavedActorChunkSavedIndex {
	GENERATED_BODY()
	bool saved = false;
	FVector position;
};

USTRUCT()
struct FSavedActorContainer {
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	UClass* ActorClass;

	UPROPERTY(SaveGame)
	FString ActorName;

	UPROPERTY(SaveGame)
	FTransform ActorTransform;

	UPROPERTY(SaveGame)
	TArray<uint8> BinaryData;

	UPROPERTY()
	int64 ActorSavableUID;
};

UENUM()
enum AChunkSavableActorState {
	TRACKING, // object will be saved over time and in EndPlay
	TRACKING_PAUSED_BY_STREAMING_UNLOAD,
	LOAD_OR_INIT_NEEDED, // object will wait for outer call
	NEED_TO_DESTROY_WITHOUT_HANDLING // actor should not exist, do not handle any saving or deletion
};

UCLASS()
class NOCUBETECH_API AChunkSavableActor : public AActor
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<AGlobalChunkRegistry> chunkRegistry;

	AChunkSavableActorState state = LOAD_OR_INIT_NEEDED;

	UPROPERTY()
	uint64 chunkSavedActorUniqueId = 0; // zero by default, but zero is valid only before 'init' or load

	FSavedActorChunkSavedIndex currentlySavedIndex;

	float lastSavedTime = 0;

	void fillContainer(FSavedActorContainer& container);

	// if id in chunkRegistry as loaded, destroy self, else start tracking
	void startTracking();
	void pauseTracking();

protected:
	USceneComponent* rootSceneComponent;
	virtual void loadFromArchive(FArchive& archive) {};

	virtual void saveToArchive(FArchive& archive) {
		archive << chunkSavedActorUniqueId;
	};

	virtual void setupByDefault() { return; };
	
	void tryFindChunkRegistry();


	virtual float GetSaveInterval() {
		return 60;
	}


public:
	AChunkSavableActor();
	/*
	* If not saved anywhere: try find new dst
	* If was saved: check is new dst == old dst, remove from old dst, save to new dst
	*/
	bool TrySaveToChunkRegistryDataUniquely(FVector position);
	bool TryDeleteFromChunkRegistryData();

	FString GetNameUsingId();


	void SetupByLoading(FArchive& archive, AGlobalChunkRegistry* chunkRegistry_) {
		check(state == LOAD_OR_INIT_NEEDED);
		check(chunkSavedActorUniqueId == 0);
		chunkRegistry = chunkRegistry_;
		archive << chunkSavedActorUniqueId;
		check(chunkSavedActorUniqueId != 0);
		loadFromArchive(archive);
		SetActorLabel(GetNameUsingId());
		Rename(GetNameUsingId().GetCharArray().GetData());
		startTracking();
		currentlySavedIndex.saved = true;
		currentlySavedIndex.position = GetActorLocation();
	}

	// call virtual setupByDefault and start tracking
	void SetupByDefault(AGlobalChunkRegistry* chunkRegistry_)
	{
		check(state == LOAD_OR_INIT_NEEDED);
		check(chunkSavedActorUniqueId == 0);
		chunkRegistry = chunkRegistry_;
		chunkSavedActorUniqueId = FDateTime::UtcNow().GetTicks();
		setupByDefault();
		SetActorLabel(GetNameUsingId());
		Rename(GetNameUsingId().GetCharArray().GetData());
		startTracking();
	};

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool IsTracking() {
		return state == TRACKING;
	}
};
