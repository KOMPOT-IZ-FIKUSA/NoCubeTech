// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractAddObjSpecification.h"
#include "LandscapeAddObjSpecification.generated.h"

/**
 *
 */
UCLASS()
class NOCUBETECH_API ULandscapeAddObjSpecification : public UAbstractAddObjSpecification
{
	GENERATED_BODY()

	/*
	* If an object exists at the given coordinate, sets the heightResult to the height of the top point and returns true.
	* Othwerwise returns false.
	*/
	virtual bool GetHeightIfExistsAt(float x, float y, float& heightResult) {
		PURE_VIRTUAL(AAdditionalObject::GetHeightIfExistsAt, return false;);
	}
};
