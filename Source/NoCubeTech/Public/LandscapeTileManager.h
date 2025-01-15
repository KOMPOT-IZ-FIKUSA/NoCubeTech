// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "UObject/ObjectPtr.h"
#include "ProceduralMeshComponent.h"
#include "GlobalWorldGenerator.h"
#include "WorldGenerator.h"
#include "LandscapeTileManager.generated.h"


/**
 * Represents a tile of procedural landscape.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOCUBETECH_API ULandscapeTileManager : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULandscapeTileManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Sets up the mesh (positions and uv)
	void SetupGeometry();

	// Creates an instance of TextureGenerationCache and starts the generation
	void SetupMaterial();

	void CheckForProceduralComponentAndCreateIfNotPresent();

	// Deletes invalid pawns
	void ValidateTrackedPawns();
	
	/*
	* Calculates the excepced number of details.
	* If CurrentDetails < expected then sets CurrentDetails = calculatedDetails and calls SetupGeometry and StartSetupMaterial()
	*/
	void SetupLandscapeIfNecessary();


	int CalculateDetails() const;

	UTexture2D* CreateTextureBGRAWithMips(uint8* data, int width, int height, FName textureName);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "MaterialInterface"))
	TObjectPtr<UMaterialInterface> materialBase;

	UPROPERTY()
	TObjectPtr<UProceduralMeshComponent> meshComponent;

	// A list of pawns distances to which are considered for choosing quality when setting geometry
	UPROPERTY()
	TArray<TWeakObjectPtr<APawn>> trackedPawns;

	
	void FindWorldGenerator();

	// necessary to initialize from outer init code for naming sub-objects
	UPROPERTY()
	int chunkX = 0;
	// necessary to initialize from outer init code for naming sub-objects
	UPROPERTY()
	int chunkY = 0;

	UPROPERTY(EditAnywhere)
	int MinTileDetails = 4;

	UPROPERTY(EditAnywhere)
	int MaxTileDetails = 64;
	
	/*
	* The distance where the details start to decrease
	*/
	UPROPERTY(EditAnywhere)
	float DetailsDistanceUnit = 128;

	/*
	* If update every tick (which is very bad) is calculated with formula:
	* CurrentDetails = MaxTileDetails * min(1, DetailsDistanceUnit / distance)
	*/
	UPROPERTY()
	int CurrentDetails = -1;

	/*
	* Number of pixels side for a square side.
	* For example, if CurrentDetails = 16 and PixelsBySquare = 4 then
	* the total number of squares would be 16*16
	* and the total number of pixels would be 256*(4*4)
	*/
	UPROPERTY()
	int PixelsBySquare = 2;

	UPROPERTY(EditAnywhere)
	bool UpdateAutomaticaly = true;

	UPROPERTY(EditAnywhere)
	float TileSize = 8192;

	UPROPERTY()
	TWeakObjectPtr<AGlobalWorldGenerator> worldGenerator;

};