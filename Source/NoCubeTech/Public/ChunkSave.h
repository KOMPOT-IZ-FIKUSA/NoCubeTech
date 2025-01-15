// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "Kismet/GameplayStatics.h"
#include "ChunkSave.generated.h"

UCLASS()
class NOCUBETECH_API UUniversalChunkDataContainer : public UObject {
	GENERATED_BODY()
private:
	UChunkSaveData* chunkDataOwner = nullptr;

public:

	void SetChunkDataChanged();
	void SetChunkDataAccessed();
	bool IsChunkDataValid();
	void SetUp(UChunkSaveData* chunkDataOwner_);

};

UCLASS()
class NOCUBETECH_API UChunkSaveData : public USaveGame
{
	GENERATED_BODY()

private:

	

	UPROPERTY()
	TMap<FString, UUniversalChunkDataContainer*> dataContainers = {};
	
	UPROPERTY()
	TMap<FString, UClass*> dataContainersClasses = {};

	UWorld* worldContextObject = nullptr;

	bool dirty = true;

	float lastAccessedTime = 0;

	UClass* findContainerClass(FString classPathName) {
		UClass** foundClass = dataContainersClasses.Find(classPathName);
		if (foundClass != nullptr) {
			return *foundClass;
		}
		else {

			UClass* SaveGameClass = UClass::TryFindTypeSlow<UClass>(classPathName);
			if (SaveGameClass == nullptr)
			{
				SaveGameClass = LoadObject<UClass>(nullptr, *classPathName);
			}
			dataContainersClasses.Add(classPathName, SaveGameClass);
			return SaveGameClass;
		}
	}

public:

	void SetDirty() {
		dirty = true;
		UpdateAccessed();
	}

	bool IsChunkDataValid() {
		return worldContextObject != nullptr;
	}

	void Invalidate() {
		worldContextObject = nullptr;
	}

	void UpdateAccessed() {
		lastAccessedTime = UGameplayStatics::GetTimeSeconds(worldContextObject);
	}

	void SetUp(UWorld* worldContextObject_) {
		check(worldContextObject_ != nullptr);
		check(worldContextObject == nullptr);
		worldContextObject = worldContextObject_;
		UpdateAccessed();
	}

	float GetLastAccessedSeconds() {
		return lastAccessedTime;
	}

	UUniversalChunkDataContainer* GetDataContainer(FString key, UClass* class_) {

		UUniversalChunkDataContainer** universalContainer = dataContainers.Find(key);
		
		if (universalContainer != nullptr) {
			return *universalContainer;
		}
		else {
			UUniversalChunkDataContainer* universalContainer1 = NewObject<UUniversalChunkDataContainer>(GetTransientPackage(), class_);
			universalContainer1->SetUp(this);
			dataContainers.Add(key, universalContainer1);
			return universalContainer1;
		}
	};

	bool SaveToSlotIfDirty(FString slotName) {
		if (dirty) {
			return UGameplayStatics::SaveGameToSlot(this, slotName, 0);
			dirty = false;
		}
		else {
			return true;
		}
	}

	void Serialize(FArchive& ar) override {
		if (ar.IsLoading()) {
			FString key;
			ar << key;
			FString containerClassPathName;
			ar << containerClassPathName;
			UClass* containerClass = findContainerClass(containerClassPathName);
			check(containerClass != nullptr);
			UUniversalChunkDataContainer* chunkDataObj = NewObject<UUniversalChunkDataContainer>(GetTransientPackage(), containerClass);
			chunkDataObj->SetUp(this);
			chunkDataObj->Serialize(ar);
			dataContainers.Add(key, chunkDataObj);
			dirty = false;
		}
		else {
			for (auto pair : dataContainers) {
				FString containerClassPathName = pair.Value->GetClass()->GetPathName();
				ar << pair.Key;
				ar << containerClassPathName;
				pair.Value->Serialize(ar);
			}
		}
	}

};
