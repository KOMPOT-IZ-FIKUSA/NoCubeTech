// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"

/**
 * non-serializable
 */

class AAdditionalObject;
class WorldGenerator;
class AGlobalChunkRegistry;

class NOCUBETECH_API AbstractAdditionalObjGenerator
{

private:
	int id = -1;


public:
	AbstractAdditionalObjGenerator();

	/*
	* The generation priority chosen for this object type. Lower means the object is going to be generated earier.
	*/
	virtual float GetPriority() const {
		PURE_VIRTUAL(AbstractAdditionalObjGenerator::GetPriority, return 0;);
	}

	/*
	* Create an object specification
	*/
	virtual AAdditionalObject* CreateActor(UWorld* world, AGlobalChunkRegistry* chunkRegistry, WorldGenerator& worldGenerator, float x, float y, UObject* outer) const {
		PURE_VIRTUAL(AbstractAdditionalObjGenerator::CreateActor, return nullptr;);
	}

	virtual TArray<FVector2D> GeneratePositionsForChunk(WorldGenerator& worldGenerator, float centerX, float centerY, float chunkSize) const
		PURE_VIRTUAL(AbstractAdditionalObjGenerator::GeneratePositionsForChunk, return TArray<FVector2D>(););

	void SetRegistered(int id_) {
		check(!IsRegistered());
		id = id_;
	}

	int GetId() {
		check(IsRegistered());
		return id;
	}

	bool IsRegistered() {
		return id != -1;
	}

};
