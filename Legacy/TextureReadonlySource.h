// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// non-serializable
struct SimpleTextureContainer {
	int width;
	int height;
	FColor* colors;

	SimpleTextureContainer(int width, int height, FColor* colors)
		: width(width), height(height), colors(colors)
	{
	}

	inline FColor AccessByXY(int x, int y) {
		return colors[y * width + x];
	}
};

/**
 * Source of FColor that is used for procedural generation
 * 
 * non-serializable
 */
class NOCUBETECH_API TextureReadonlySource
{
private:
	TArray<SimpleTextureContainer> data;

public:
	/*
	* Creates all SimpleTextureContainer instances in data variable
	* using simple average for creating mip levels
	* texture must have no mipmap and VectorDesplacementMap compression mode
	*/
	TextureReadonlySource(UTexture2D* texture);
	
	~TextureReadonlySource();

	/**
	 * @param scale The ratio (pixel size on result texture / pixel size on this texture)
	 */
	FColor GetColorNearest(float x, float y, float scale);


protected:
	/*
	* Returns values from range [0; 13] = [0, 14)
	*/
	inline int FastIntLogarithm(float val) {

		int i;
		if (val < 2) {
			i = 0;
		}
		else if (val < 4) {
			i = 1;
		}
		else if (val < 8) {
			i = 2;
		}
		else if (val < 16) {
			i = 3;
		}
		else if (val < 32) {
			i = 4;
		}
		else if (val < 64) {
			i = 5;
		}
		else if (val < 128) {
			i = 6;
		}
		else if (val < 256) {
			i = 7;
		}
		else if (val < 512) {
			i = 8;
		}
		else if (val < 1024) {
			i = 9;
		}
		else if (val < 2048) {
			i = 10;
		}
		else if (val < 4096) {
			i = 11;
		}
		else if (val < 8192) {
			i = 12;
		}
		else {
			i = 13;
		}
		return i;
	}
	/*
	* @param i Value from range [0; 13] = [0, 14)
	*/
	inline float FastIntPower(int i) {
		float p;
		if (i <= 0) {
			p = 1;
		}
		else if (i == 1) {
			p = 2;
		}
		else if (i == 2) {
			p = 4;
		}
		else if (i == 3) {
			p = 8;
		}
		else if (i == 4) {
			p = 16;
		}
		else if (i == 5) {
			p = 32;
		}
		else if (i == 6) {
			p = 64;
		}
		else if (i == 7) {
			p = 128;
		}
		else if (i == 8) {
			p = 256;
		}
		else if (i == 9) {
			p = 512;
		}
		else if (i == 10) {
			p = 1024;
		}
		else if (i == 11) {
			p = 2048;
		}
		else if (i == 12) {
			p = 4096;
		}
		else {
			p = 8192;
		}
		return p;

	}
};
