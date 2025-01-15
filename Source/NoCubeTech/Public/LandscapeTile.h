// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkSavableActor.h"
#include "LandscapeTileManager.h"
#include "LandscapeTile.generated.h"

/**
 *
 */
UCLASS()
class NOCUBETECH_API ALandscapeTile : public AChunkSavableActor
{
	GENERATED_BODY()

public:

	ALandscapeTile();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Sets up the mesh (positions and uv)
	void SetupGeometry();

	void SetupMaterial();

	void CheckForProceduralComponentAndCreateIfNotPresent();

	/*
	* Calculates the excepced number of details.
	* If CurrentDetails < expected then sets CurrentDetails = calculatedDetails and calls SetupGeometry and StartSetupMaterial()
	*/
	void SetupLandscapeIfNecessary();

	int CalculateDetails() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "MaterialInterface"))
		TObjectPtr<UMaterialInterface> materialBase;

	UPROPERTY()
		TObjectPtr<UProceduralMeshComponent> mesh;

	void FindWorldGenerator();

	UPROPERTY(EditAnywhere)
		int MinTileDetails = 8;

	UPROPERTY(EditAnywhere)
		int MaxTileDetails = 32;

	/*
	* The distance where the details start to decrease
	*/
	UPROPERTY(EditAnywhere)
		float DetailsDistanceUnit = 6000;

	/*
	* Calculated with formula:
	* CurrentDetails = MaxTileDetails * min(1, DetailsDistanceUnit / distance) -> then found closest power of 2
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

	UPROPERTY(EditAnywhere, Replicated)
		float TileSize = -1;

	UPROPERTY(EditAnywhere, Replicated)
		int32 xIndex = -1;

	UPROPERTY(EditAnywhere, Replicated)
		int32 yIndex = -1;

	UPROPERTY()
		TWeakObjectPtr<AGlobalWorldGenerator> worldGenerator;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	void setupByDefault() override {
		AGlobalChunkRegistry* chunkRegistry_ = getChunkRegistry();
		FVector selfPosition = GetActorLocation();
		TileSize = chunkRegistry_->WorldPartitionCellSize;
		xIndex = chunkRegistry_->PositionToIndex(selfPosition.X);
		yIndex = chunkRegistry_->PositionToIndex(selfPosition.Y);

	};


	virtual void loadFromArchive(FArchive& archive) override {
		AGlobalChunkRegistry* chunkRegistry_ = getChunkRegistry();
		FVector selfPosition = GetActorLocation();
		TileSize = chunkRegistry_->WorldPartitionCellSize;
		xIndex = chunkRegistry_->PositionToIndex(selfPosition.X);
		yIndex = chunkRegistry_->PositionToIndex(selfPosition.Y);
	};

	virtual void saveToArchive(FArchive& archive) override {
	};

	// Needed to redefine relevancy by XYZ distance by relevancy by XY distance
	bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override {
		float trueDist = NetCullDistanceSquared;
		
		float& referenceToNetCullDistanceSquared = *(float*)(void*)&NetCullDistanceSquared; // Temporarily breaking the const

		referenceToNetCullDistanceSquared = 1e30; // Set to a huge value - distance does not matter anymore

		bool result = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
		referenceToNetCullDistanceSquared = trueDist; // Set back to true value
		
		if (result) {
			return  FVector::DistSquaredXY(SrcLocation, GetActorLocation()) < NetCullDistanceSquared * 2; // Just multiplying by 2 to make sure all the close chunks will be loaded
		}
		else {
			return false;
		}
	};

};
