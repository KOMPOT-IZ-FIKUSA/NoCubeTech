// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomesConst.h"

/*
 Non-serializable
*/
struct NOCUBETECH_API BiomeWeights {
	float values[BIOMES_COUNT];

	BiomeWeights() {

	}
	~BiomeWeights() {
	}

	/*
	* Applies a transformation to set the sum of weights to 1 and make all weights positive.
	* betta : the greater is betta the sharper is edge between biomes
	*/
	void ApplyNormalization(float betta) {
		// using softmax
		double sumExp = 0;
		float min_ = values[0];
		float max_ = values[0];
		for (int i = 0; i < BIOMES_COUNT; i++) {
			min_ = fmin(min_, values[i]);
			max_ = fmax(max_, values[i]);
		}
		float avg = (min_ + max_) / 2;
		TArray<double> exps;
		for (int i = 0; i < BIOMES_COUNT; i++) {
			double exp_ = exp(fmin(700, fmax(-700, (values[i] - avg) * betta)));
			exps.Add(exp_);
			sumExp += exp_;
		}
		for (int i = 0; i < BIOMES_COUNT; i++) {
			values[i] = exps[i] / sumExp;
		}
	}


	static BiomeWeights Lerp(const BiomeWeights& one, const BiomeWeights& two, float weight) {
		BiomeWeights result = BiomeWeights();
		float inv = 1 - weight;
		for (int i = 0; i < BIOMES_COUNT; i++) {
			result.values[i] = one.values[i] * inv + two.values[i] * weight;
		}
		return result;
	}

	static BiomeWeights* LerpPtr(BiomeWeights* one, BiomeWeights* two, float weight) {
		BiomeWeights * result = new BiomeWeights();
		float inv = 1 - weight;
		for (int i = 0; i < BIOMES_COUNT; i++) {
			result->values[i] = one->values[i] * inv + two->values[i] * weight;
		}
		return result;
	}

};