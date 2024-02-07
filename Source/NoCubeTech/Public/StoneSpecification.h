// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbstractAddObjSpecification.h"
#include "StoneSpecification.generated.h"

/**
 * 
 */
UCLASS()
class NOCUBETECH_API UStoneSpecification : public UAbstractAddObjSpecification
{
	GENERATED_BODY()
public:
	UStoneSpecification();
	~UStoneSpecification();

	UPROPERTY()
	FVector position;
	UPROPERTY()
	float radius;
};
