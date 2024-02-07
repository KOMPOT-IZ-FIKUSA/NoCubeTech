// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/NameAsStringProxyArchive.h"
#include "ChunkSavableActor.h"
#include "ChunkSave.generated.h"

UCLASS()
class NOCUBETECH_API UChunkSaveData : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray< FSavedActorContainer> SavedActors;
};
