// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeAdditionalObject.h"
#include "ActorListGrid.h"
#include "ProceduralMeshComponent.h"
#include "FlatTerrainPart.generated.h"

/**
 * 
 */
UCLASS()
class NOCUBETECH_API AFlatTerrainPart : public ALandscapeAdditionalObject
{
	GENERATED_BODY()

public:


	UPROPERTY()
	TArray<int> faces;

	UPROPERTY()
	TArray<FVector> normals;

	UPROPERTY()
	TArray<FVector> vertices;

	UPROPERTY()
	TArray<FVector2D> uvs;


	UPROPERTY()
	UProceduralMeshComponent* mesh;

	AFlatTerrainPart();
	
	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	virtual bool GetAbsoluteLandscapeHeight(float x, float y, float& out) override;


protected:
	void loadFromArchive(FArchive& ar) override;
	void saveToArchive(FArchive& ar) override;

	static void DistortFinalMeshCoordinate(FVector& vector);

	static TArray<FVector2D> Create2DRandomFigure(int64 seedForThisObject, float chinkSize);

	void setupByDefault() override;

	void calculateBoundsGlobal(FVector& min_, FVector& max_);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void CreateGeometryOfOneStone(TArray<FVector>& vertices, TArray<int>& faces, TArray<FVector>& normalsOut, int seed);


private:
	bool geometrySet;
	StorableObjectReference storableReference;


	static const float MIN_BASE_SCALE;
	static const float MAX_BASE_SCALE; // can be reduced in runtime in algorithms due to chunk size

};
