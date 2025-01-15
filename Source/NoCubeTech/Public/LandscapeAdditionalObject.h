// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AAdditionalObject.h"
#include "LandscapeAdditionalObject.generated.h"

/**
 * 
 */
UCLASS()
class NOCUBETECH_API ALandscapeAdditionalObject : public AAdditionalObject
{
	GENERATED_BODY()

public:
	virtual bool GetAbsoluteLandscapeHeight(float x, float y, float& out) {
		PURE_VIRTUAL(ALandscapeAdditionalObject::GetAbsoluteLandscapeHeight, return false;);
	};
	
};
