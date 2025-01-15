// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalChunkRegistry.h"
#include "AbstractAddObjSpecification.generated.h"


class AAdditionalObject;
/**
 * 
 */
UCLASS()
class NOCUBETECH_API UAbstractAddObjSpecification : public UObject
{
	GENERATED_BODY()
public:
	UAbstractAddObjSpecification();
	~UAbstractAddObjSpecification();

	// Checks if this can exist in presence of another. For example, stones can exist wherever trees are, but trees cannot exist inside stones
	virtual bool CanExistWithAnother(UAbstractAddObjSpecification* other) const {
		return true;
	}

	virtual TArray<FString> GetTags() const {
		return TArray<FString>();
	}

	// finalize the generation process and return the generated actor
	virtual AAdditionalObject* CreateObject(AGlobalChunkRegistry* chunkRegistry) {
		return nullptr;
	}

	virtual void SerializeSpecs(FArchive& ar) {

	}

};
