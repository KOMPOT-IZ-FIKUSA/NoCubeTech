// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractAdditionalObjGenerator.h"
#include "StoneSpecification.h"

/**
 * 
 */
class NOCUBETECH_API AdditionalStoneGenerator : public AbstractAdditionalObjGenerator
{


public:
	AdditionalStoneGenerator();
	virtual ~AdditionalStoneGenerator();

	virtual float GetPriority() const override {
		return 0;
	}

	virtual bool CanBeGenerated(AdditionalObjectGenerationData* data) const override {
		return RandomGenerator::Int_x_y_seed_ToFloat(data->x, data->y, data->seed) > 0.8;
	};

	virtual UAbstractAddObjSpecification* GenerateSpecification(const AdditionalObjectGenerationData& data, UObject* outer = nullptr) const override {
		UStoneSpecification* stone = NewObject< UStoneSpecification>(outer);
		stone->position = FVector(data.x, data.y, data.groundHeight);
		stone->radius = (1 + RandomGenerator::IntToFloat(data.x) * 5) * 100;
		return stone;
	};

};
