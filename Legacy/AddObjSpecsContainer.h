// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractAddObjSpecification.h"
#include "AddObjSpecsContainer.generated.h"

/**
 * A serialable wrapper for UAbstractAddObjSpecifications
 */
USTRUCT()
struct NOCUBETECH_API FAddObjSpecsContainer
{
	GENERATED_BODY()
public:
	FAddObjSpecsContainer();
	~FAddObjSpecsContainer();

	UPROPERTY(SaveGame)
	FString SpecsClassName;

	UPROPERTY(SaveGame)
	TArray<uint8> BinaryData;

	void Put(UAbstractAddObjSpecification* specs) {
		SpecsClassName = specs->GetClass()->GetName();
		BinaryData = TArray<uint8>();
		FMemoryWriter ar = FMemoryWriter(BinaryData);
		specs->SerializeSpecs(ar);
	}

	// TODO: exception handling
	UAbstractAddObjSpecification* InstantiateSpecs(UObject* outer, const TMap<FString, UClass*>& specsClasses) {
		UClass*const* specsClassFound = specsClasses.Find(SpecsClassName);
		if (!specsClassFound || !(*specsClassFound)) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot find UAbstractAddObjSpecification class during FAddObjSpecsContainer::InstantiateSpecs."));
			return nullptr;
		}
		UClass* specsClass = *specsClassFound;
		UAbstractAddObjSpecification* result = Cast<UAbstractAddObjSpecification>(NewObject<UObject>(outer, specsClass));
		if (!result) {
			GEngine->AddOnScreenDebugMessage(-1, 60, FColor::Red, TEXT("Cannot create UAbstractAddObjSpecification instance in FAddObjSpecsContainer::InstantiateSpecs."));
			return nullptr;
		}
		FMemoryReader ar = FMemoryReader(BinaryData);
		result->SerializeSpecs(ar);
		return result;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FAddObjSpecsContainer& container) {
		Ar << container.SpecsClassName;
		Ar << container.BinaryData;
		return Ar;
	}

};
