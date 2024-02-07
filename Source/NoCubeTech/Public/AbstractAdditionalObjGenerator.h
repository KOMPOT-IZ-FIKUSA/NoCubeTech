// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesWeights.h"
#include "BiomeGenerationData.h"
#include "AbstractAddObjSpecification.h"

/**
 * non-serializable
 */
class NOCUBETECH_API AbstractAdditionalObjGenerator
{
public:
	AbstractAdditionalObjGenerator();

	/*
	* The generation priority chosen for this object type. Lower means the object is going to be generated earier.
	*/
	virtual float GetPriority() const {
		return 0;
	}

	/*
	* This method estimates the probility and possibility for the new object to be generated.
	*/
	virtual bool CanBeGenerated(AdditionalObjectGenerationData* data) const {
		check(false); // not implemented
		return false;
	};

	/*
	* Create an object specification
	*/
	virtual UAbstractAddObjSpecification* GenerateSpecification(const AdditionalObjectGenerationData& data, UObject* outer = nullptr) const {
		check(false); // not implemented
		return nullptr;
	};

};
