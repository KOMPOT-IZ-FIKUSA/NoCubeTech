// Fill out your copyright notice in the Description page of Project Settings.


#include "StoneSpecification.h"

UStoneSpecification::UStoneSpecification()
{
	position = FVector::ZeroVector;
	faces = {};
	vertices = {};
}

UStoneSpecification::~UStoneSpecification()
{
}

bool UStoneSpecification::GetHeightIfExistsAt(float x, float y, float& resultHeight) {
	return false;
}
