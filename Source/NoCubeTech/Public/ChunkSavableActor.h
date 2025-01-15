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

class SimpleStaticIncrementHelper {
public:
	static int incrementingVariable; // needed to be able to create two savable actors with different ids at the same time. Limited with 10000.
	static int getIncrementResult() {
		if (incrementingVariable < 0 || incrementingVariable > 9999) {
			incrementingVariable = 0;
		}
		else {
			incrementingVariable += 1;
		}
		return incrementingVariable;
	}
};

UENUM()
enum FDestroySavableActorMode {
	DELETE,
	SAVE,
	NONE
};

UCLASS()
class NOCUBETECH_API AChunkSavableActor : public AActor
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<AGlobalChunkRegistry> chunkRegistry;

	UPROPERTY()
	uint64 chunkSavedActorUniqueId = 0; // zero by default, but zero is valid only before 'init' or load. Zero state means object is not set up (by default or loaded)

	FSavedActorChunkSavedIndex currentlySavedIndex;

	float lastSavedTime = 0;

	void fillContainer(FSavedActorContainer& container);


protected:
	AGlobalChunkRegistry* getChunkRegistry() {
		return chunkRegistry.Get();
	}

	USceneComponent* rootSceneComponent;
	virtual void loadFromArchive(FArchive& archive) {};

	virtual void saveToArchive(FArchive& archive) {
	};

	virtual void setupByDefault() { return; };
	
	void tryFindChunkRegistry();


	virtual float GetSaveInterval() {
		return 600;
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

	void DestroySavable(FDestroySavableActorMode mode);

	void SetupByLoading(FArchive& archive, AGlobalChunkRegistry* chunkRegistry_) {
		if (!HasAuthority()) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to load AChunkSavableActor from an archive without authority in AChunkSavableActor::SetupByLoading. Destroying..."));
			Destroy();
			return;
		}
		if (chunkSavedActorUniqueId != 0) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to load AChunkSavableActor from an archive when is already set up in AChunkSavableActor::SetupByLoading. Destroying..."));
			Destroy();
			return;
		}
		archive << chunkSavedActorUniqueId;
		chunkRegistry = chunkRegistry_;
		if (chunkSavedActorUniqueId == 0) { // chunkSavedActorUniqueId must be zero (invalid) before loading, but non-zero after loading
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Loaded AChunkSavableActor with 0 id. Destroying..."));
			Destroy();
			return;
		}
		loadFromArchive(archive);
		SetActorLabel(GetNameUsingId());
		chunkRegistry->SetSavableActorTracked(chunkSavedActorUniqueId);
		NetCullDistanceSquared = chunkRegistry_->WorldPartitionLoadRange * chunkRegistry_->WorldPartitionLoadRange;
		currentlySavedIndex.saved = true;
		currentlySavedIndex.position = GetActorLocation();
	}

	// call virtual setupByDefault and start tracking
	void SetupByDefault(AGlobalChunkRegistry* chunkRegistry_)
	{
		if (!HasAuthority()) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to setup AChunkSavableActor by default without authority in AChunkSavableActor::SetupByDefault. Destroying..."));
			Destroy();
			return;
		}
		if (chunkSavedActorUniqueId != 0) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Trying to load AChunkSavableActor from an archive when is already set up in AChunkSavableActor::SetupByLoading"));
			Destroy();
			return;
		}
		chunkRegistry = chunkRegistry_;
		chunkSavedActorUniqueId = FDateTime::UtcNow().GetTicks() + SimpleStaticIncrementHelper::getIncrementResult();

		if (chunkSavedActorUniqueId == 0) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Set up new AChunkSavableActor with 0 id. Destroying..."));
			Destroy();
			return;
		}

		currentlySavedIndex.saved = false;
		setupByDefault();
		SetActorLabel(GetNameUsingId());
		chunkRegistry->SetSavableActorTracked(chunkSavedActorUniqueId);
		NetCullDistanceSquared = chunkRegistry_->WorldPartitionLoadRange * chunkRegistry_->WorldPartitionLoadRange;
	};

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	int64 GetSavableUniqueId() {
		return chunkSavedActorUniqueId;
	}

	bool IsSavableValid() {
		if (!HasAuthority()) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Yellow, TEXT("AChunkSavableActor::IsSavableValid called without authority"));
		}
		return chunkSavedActorUniqueId != 0;
	}
};
