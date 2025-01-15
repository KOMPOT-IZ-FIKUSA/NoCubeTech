// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "RandomGenerator.h"

class NOCUBETECH_API AbstractBiome {
private:
	int id;
public:
	virtual float GenerateHeight(float x, float y, int64 seed) = 0;

	AbstractBiome(int id) : id(id) {
	}

	int GetId() { return id; }

};
