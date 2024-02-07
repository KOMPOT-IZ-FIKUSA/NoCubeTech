// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TextureReadonlySource.h"

/**
 * An objects that contains "static" resources for procedural generation
 * 
 * non-serializable
 */
class NOCUBETECH_API GlobalColorGenerationData
{
public:
	GlobalColorGenerationData();

	TextureReadonlySource* grassTexture = nullptr;
	TextureReadonlySource* rockTexture = nullptr;
	TextureReadonlySource* rockTexture2 = nullptr;
	TextureReadonlySource* rockTexture3 = nullptr;
	TextureReadonlySource* dirtTexture1 = nullptr;

	~GlobalColorGenerationData() {
		if (grassTexture) {
			delete grassTexture;
		}
		if (rockTexture) {
			delete rockTexture;
		}
		if (rockTexture2) {
			delete rockTexture2;
		}
		if (rockTexture3) {
			delete rockTexture3;
		}
		if (dirtTexture1) {
			delete dirtTexture1;
		}
	};

};
